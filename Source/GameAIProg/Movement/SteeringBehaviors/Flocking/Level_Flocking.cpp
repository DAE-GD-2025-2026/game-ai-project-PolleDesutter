// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_Flocking.h"


// Sets default values
ALevel_Flocking::ALevel_Flocking()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_Flocking::BeginPlay()
{
	Super::BeginPlay();

	TrimWorld->SetTrimWorldSize(1700.f);
	TrimWorld->bShouldTrimWorld = true;
	
	pAgentToEvade = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector(0, 0, 90), FRotator::ZeroRotator);
	if (!pAgentToEvade)
	{
		UE_LOG(LogTemp, Error, TEXT("AgentToEvade is invalid"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;
	}
	

	pFlock = TUniquePtr<Flock>(
		new Flock(
			GetWorld(),
			SteeringAgentClass,
			FlockSize,
			TrimWorld->GetTrimWorldSize(),
			pAgentToEvade,
			true)
			);
}

// Called every frame
void ALevel_Flocking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	pFlock->ImGuiRender(WindowPos, WindowSize);
	pFlock->Tick(DeltaTime);
	pFlock->RenderDebug();
	
	if (bUseMouseTarget)
	{
		UE_LOGFMT(LogTemp, Verbose, "MouseTarget Location: {Location}", MouseTarget.Position.ToString());
		pFlock->SetTarget_Seek(MouseTarget);
	}
	
	// Update Evade Agent's Seek Right Mouse Button

	HandleRightMouseInput();
	
}


void ALevel_Flocking::HandleRightMouseInput() const
{
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	FVector MouseWorldPosition{};
	const bool IsMouseWorldPosValid = UnrealHelpers::GetMouseWorldPosition(GetWorld(), PC, MouseWorldPosition);
	if (!IsMouseWorldPosValid)
	{
		return;
	}
	
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController not valid"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;
	}

	if (PC->IsInputKeyDown(EKeys::RightMouseButton))
	{
		DrawDebugPoint(GetWorld(), MouseWorldPosition, 10.f, FColor::Black);
		UE_LOGFMT(LogTemp, Verbose, "Input LeftMouse Raycast HitLocation: {HitLocation}",
		          *MouseWorldPosition.ToString());

		FSteeringParams TargetData{};
		TargetData.Position = FVector2D(MouseWorldPosition.X, MouseWorldPosition.Y);
		TargetData.Orientation = 0.f;
		TargetData.LinearVelocity = FVector2D(0.f, 0.f);
		TargetData.AngularVelocity = 0.f;
		
		pFlock->SetTarget_SeekEvadeAgent(TargetData);
	}
}

