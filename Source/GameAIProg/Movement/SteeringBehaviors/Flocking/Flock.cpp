#include "Flock.h"

#include "Level_Flocking.h"
#include "Shared/WorldTrimVolume.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(UWorld* pWorld, TSubclassOf<ASteeringAgent> AgentClass, int FlockSize,
             float WorldSize, ASteeringAgent* const pAgentToEvade, bool bTrimWorld)
	: pWorld{pWorld}
	  , SteeringAgentClass{AgentClass}
	  , FlockSize{FlockSize}
	  , pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);
	Neighbors.SetNum(FlockSize - 1);

	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pVelMatchBehavior = std::make_unique<VelocityMatch>(this);

	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pEvadeNearbyBehavior = std::make_unique<EvadeNearby>();


	if (!pWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseLevelScriptActor is not valid"))
		return;
	}

	if (!pAgentToEvade)
	{
		UE_LOG(LogTemp, Warning, TEXT("AgentToEvade is not valid"))
		return;
	}

	pAgentToEvade->SetSteeringBehavior(pSeekBehavior.get());

	const auto Level = pWorld->GetLevel(0);
	if (Level)
	{
		const auto LevelScriptActor = Level->GetLevelScriptActor();
		if (LevelScriptActor)
		{
			BaseLevelScriptActor = Cast<ALevel_Base>(LevelScriptActor);
		}
	}

	if (BaseLevelScriptActor)
	{
		pTrimWorld = BaseLevelScriptActor->GetTrimWorld();
	}

	if (!BaseLevelScriptActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseLevelScriptActor is not valid"))
	}
	if (!pTrimWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("TrimWorld is not valid"))
	}


	const std::vector<BlendedSteering::WeightedBehavior> FlockWeightedBehaviors
	{
		{pCohesionBehavior.get(), 0.5f},
		{pSeparationBehavior.get(), 0.5f},
		{pVelMatchBehavior.get(), 0.5f},
	};

	pBlendedSteering = std::make_unique<BlendedSteering>(FlockWeightedBehaviors);

	const std::vector<ISteeringBehavior*> PrioritySteeringBehaviors
	{
		pEvadeNearbyBehavior.get(),
		pBlendedSteering.get()
	};

	pPrioritySteering = std::make_unique<PrioritySteering>(PrioritySteeringBehaviors);

	// TODO: initialize the flock and the memory pool

	for (int i = 0; i < FlockSize; ++i)
	{
		// // We don't care if bTrimWorld is set or not,
		// // we still spawn only in world size
		const FVector RandomSpawnPosition =
		{
			FMath::RandRange(-WorldSize, WorldSize),
			FMath::RandRange(-WorldSize, WorldSize),
			90
		};

		ASteeringAgent* Agent = pWorld->SpawnActor<ASteeringAgent>(SteeringAgentClass, RandomSpawnPosition,
		                                                           FRotator::ZeroRotator);

		if (!Agent)
		{
			UE_LOGFMT(LogTemp, Warning, "Agent {Index} failed to spawn at location: \t {SpawnPosition}", i,
			          RandomSpawnPosition.ToString());
			continue;
		}

		Agent->SetBodyMaterial(Agent->GetHighlightedMaterial());

		Agent->SetSteeringBehavior(pPrioritySteering.get());

		UE_LOGFMT(LogTemp, Verbose, "Agent {Index} Location:\t{Location}", i, RandomSpawnPosition.ToString());

		Agents[i] = Agent;
	}
}

Flock::~Flock()
{
	for (const auto Agent : Agents)
	{
		if (Agent)
			Agent->Destroy();
	}
}

void Flock::Tick(float DeltaTime)
{
	// TODO: update the flock
	if (pEvadeNearbyBehavior)
	{
		FTargetData TargetData;
		TargetData.Position = pAgentToEvade->GetPosition();
		TargetData.Orientation = pAgentToEvade->GetRotation();
		TargetData.LinearVelocity = pAgentToEvade->GetLinearVelocity();
		TargetData.AngularVelocity = pAgentToEvade->GetAngularVelocity();

		pEvadeNearbyBehavior->SetTarget(TargetData);
	}

	// TODO: for every agent:
	for (auto pAgent : Agents)
	{
		if (!pAgent)
		{
			continue;
		}

		// TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
		RegisterNeighbors(pAgent);
		// TODO: update the agent (-> the steering behaviors use the neighbors in the memory pool)
		pAgent->Tick(DeltaTime);

		// TODO: trim the agent to the world
		// this is already done automatically by NotifyActorEndOverlap
	}
}

void Flock::RenderDebug()
{
	// TODO: Render all the agents in the flock
	for (auto pAgent : Agents)
	{
		if (!pAgent)
		{
			return;
		}
		pAgent->SetDebugRenderingEnabled(DebugRenderSteering);
		if (DebugRenderNeighborhood)
		{
			RenderNeighborhood();
		}

		// TODO: Add Debug when spacial partitioning is done
		if (DebugRenderPartitions)
		{
		}
	}
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive,
		             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

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

		ImGui::Text("Flocking");
		ImGui::Spacing();

		if (ImGui::Checkbox("Debug Behavior", &CanDebugBehavior))
		{
			pAgentToEvade->SetDebugBehaviorEnabled(CanDebugBehavior);

			for (ASteeringAgent* Agent : Agents)
			{
				if (!Agent)
				{
					continue;
				}

				Agent->SetDebugBehaviorEnabled(CanDebugBehavior);
			}
		}

		// TODO: implement ImGUI checkboxes for debug rendering here
		if (BaseLevelScriptActor)
		{
			ImGui::Checkbox("Visualize Mouse Target", &BaseLevelScriptActor->VisualizeMouseTarget);
		}
		if (pTrimWorld)
		{
			ImGui::Checkbox("Trim World", &pTrimWorld->bShouldTrimWorld);
		}


		ImGui::Text("(Blended) Behavior Weights");
		ImGui::Spacing();

		// TODO: implement ImGUI sliders for steering behavior weights here
		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Cohesion",
			pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal;
			}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Separation",
			pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal;
			}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Alignment",
			pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight = InVal;
			}, "%.2f");


		ImGui::Text("(Priority) Behaviors");
		ImGui::Spacing();


		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Evade Radius",
			pEvadeNearbyBehavior->EvadeRadius, 0.0f, 1000.0f,
			[this](const float InVal)
			{
				pEvadeNearbyBehavior->EvadeRadius = InVal;
			}, "%.2f");


		ImGui::Spacing();


		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
	// TODO: Debug Render the neighbors for the first agent in the flock
	if (Agents.IsEmpty())
	{
		return;
	}

	ASteeringAgent* FirstAgent = Agents[0];
	if (!FirstAgent)
	{
		return;
	}

	RegisterNeighbors(FirstAgent);

	for (const auto Agent : Agents)
	{
		Agent->SetBodyMaterial(Agent->GetNormalBodyMaterial());
	}

	FirstAgent->SetBodyMaterial(FirstAgent->GetBlueBodyMaterial());

	for (int i{}; i < NrOfNeighbors; ++i)
	{
		const ASteeringAgent* Neighbor = Agents[i];
		if (!Neighbor || Neighbor == FirstAgent)
		{
			continue;
		}

		Neighbor->SetBodyMaterial(Neighbor->GetHighlightedBodyMaterial());
	}
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
	NrOfNeighbors = 0;

	// TODO: Implement
	for (const auto OtherAgent : Agents)
	{
		if (pAgent == OtherAgent)
		{
			continue;
		}

		if (!OtherAgent)
		{
			continue;
		}


		const float Distance = FVector2D::Distance(pAgent->GetPosition(), OtherAgent->GetPosition());

		if (Distance > NeighborhoodRadius)
			continue;

		Neighbors[NrOfNeighbors] = OtherAgent;
		++NrOfNeighbors;
	}
}

#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D AvgPosition = FVector2D::ZeroVector;
	int ValidAgents = 0;

	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		const auto pNeighbor = Neighbors[i];
		if (!pNeighbor)
		{
			continue;
		}

		++ValidAgents;
		AvgPosition += pNeighbor->GetPosition();
	}

	AvgPosition /= ValidAgents;

	return AvgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	if (NrOfNeighbors == 0)
	{
		return FVector2D::ZeroVector;
	}

	FVector2D AvgVelocity = FVector2D::ZeroVector;
	int ValidAgents = 0;

	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		const auto Neighbor = Neighbors[i];
		if (!Neighbor)
		{
			continue;
		}

		++ValidAgents;
		AvgVelocity += FVector2D(Neighbor->GetVelocity().X, Neighbor->GetVelocity().Y);
	}

	AvgVelocity /= ValidAgents;

	return AvgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target) const
{
	pSeekBehavior->SetTarget(Target);
}

void Flock::SetHighlightedColor(UMaterialInterface* Material)
{
}
