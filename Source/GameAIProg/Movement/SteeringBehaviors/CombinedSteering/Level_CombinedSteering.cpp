#include "Level_CombinedSteering.h"

#include "imgui.h"
#include "Shared/UnrealHelpers.h"

// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();
	
	SeekBehavior = std::make_unique<Seek>();
	WanderBehavior = std::make_unique<Wander>();
	EvadeBehavior = std::make_unique<Evade>();
	
	DrunkAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	ASteeringAgent* EvadingAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	
	if (!DrunkAgent)
	{
		UE_LOG(LogTemp, Error, TEXT("Drunk Agent Couldn't Spawn"));
		return;	
	}
		
	if (!EvadingAgent)
	{
		UE_LOG(LogTemp, Error, TEXT("Evading Agent Couldn't Spawn"));
		return;	
	}
	
	EvadingAgents.Push(EvadingAgent);

	BlendedSteering::WeightedBehavior SeekWeightedBehavior
	{
		SeekBehavior.get(), 
		0.5f
	};
	
	BlendedSteering::WeightedBehavior WanderWeightedBehavior
	{
		WanderBehavior.get(), 
		0.5f
	};
	
	
	const std::vector<BlendedSteering::WeightedBehavior> DrunkWeightedBehaviors
	{
		SeekWeightedBehavior,
		WanderWeightedBehavior,
	};
	
	const std::vector<ISteeringBehavior*> EvadePriorityBehaviors
	{
		WanderBehavior.get(),	
		EvadeBehavior.get(),	
	};
	
	// Drunk Agent:
	DrunkBlendedSteering = std::make_unique<BlendedSteering>(DrunkWeightedBehaviors);

	DrunkAgent->SetBodyMaterial(DrunkAgent->GetHighlightedMaterial());
	DrunkAgent->SetSteeringBehavior(DrunkBlendedSteering.get());
	
	DrunkAgent->SetDebugRenderingEnabled(CanDebugRender);	
	
	// Evading Agent:
	EvadePrioritySteering = std::make_unique<PrioritySteering>(EvadePriorityBehaviors);
	EvadingAgent->SetSteeringBehavior(EvadePrioritySteering.get());
	EvadingAgent->SetDebugRenderingEnabled(CanDebugRender);
	
	
}

void ALevel_CombinedSteering::BeginDestroy()
{
	Super::BeginDestroy();

}

void ALevel_CombinedSteering::HandleLeftMouseInput(const APlayerController* PlayerController, 
	const FVector& MouseWorldPosition)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not valid"));
		return;
	}
	
	if (!PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		IsHoldingLeftButton = false;
	}
	
	if (PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		if (IsHoldingLeftButton)
		{
			return;
		}
			
		IsHoldingLeftButton = true;
		
		
		DrawDebugPoint(GetWorld(), MouseWorldPosition, 10.f, FColor::Black);
		UE_LOGFMT(LogTemp, Verbose, "Input LeftMouse Raycast HitLocation: {HitLocation}", *MouseWorldPosition.ToString());

		const auto WorldVolume = UnrealHelpers::GetFirstActorOfClass<AWorldTrimVolume>(GetWorld());
		if (!WorldVolume)
		{
			UE_LOG(LogTemp, Warning, TEXT("Didn't find WorldTrimVolume"));
			return;
		}
		
		const bool IsInside = UnrealHelpers::IsPositionInsideVolume(*WorldVolume->GetTrimVolume(), MouseWorldPosition);
		UE_LOGFMT(LogTemp, Verbose, "IsInside: {InsideBoolean}", IsInside);
		
		ASteeringAgent* Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, MouseWorldPosition, FRotator::ZeroRotator);
		
		if (!Agent)
		{
			return;
		}
		
		// Add EvadePriority Behavior
		Agent->SetSteeringBehavior(EvadePrioritySteering.get());
		Agent->SetDebugRenderingEnabled(CanDebugRender);	
		
		EvadingAgents.Push(Agent);
	}
	
}


// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scroll wheel: zoom cam.");
		ImGui::Unindent();
	
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();
	
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Combined Steering");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
			DrunkAgent->SetDebugRenderingEnabled(CanDebugRender);
			
			for (ASteeringAgent* Agent : EvadingAgents)
			{
				if (!Agent)
				{
					continue;
				}
				
				Agent->SetDebugRenderingEnabled(CanDebugRender);
			}
		}
		
		ImGui::Checkbox("Visualize Mouse Target", &VisualizeMouseTarget);
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("(Drunk Agent) Behavior Weights");
		ImGui::Spacing();

		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
			DrunkBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](const float InVal)
			{
				DrunkBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal;
			}, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		DrunkBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
			[this](const float InVal)
			{
				DrunkBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal;
			}, "%.2f");
	
		//End
		ImGui::End();
	}
#pragma endregion
	
	
	// TODO: implement Make sure to also evade the wanderer
	
	
	// Combined Steering Update
	// TODO: implement handling mouse click input for seek
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}
	
	FVector MouseWorldPosition{};
	const bool IsMouseWorldPosValid = UnrealHelpers::GetMouseWorldPosition(GetWorld(), PC, MouseWorldPosition);
	if (!IsMouseWorldPosValid)
	{
		UE_LOG(LogTemp, Log, TEXT("MouseClick Didn't hit anything"));
		return;	
	}
	
	
	// Spawn Actor on Left Mouse Button
	HandleLeftMouseInput(PC, MouseWorldPosition);
	

	// Seek Target on Right Mouse Button
	if (PC->IsInputKeyDown(EKeys::RightMouseButton))
	{
		MouseTarget.Position = FVector2D(MouseWorldPosition.X, MouseWorldPosition.Y);
		SeekBehavior->SetTarget(MouseTarget);
	}
	
	
	
}
