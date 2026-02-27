// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"
#include "CombinedSteeringBehaviors.h"
#include "GameAIProg/Shared/Level_Base.h"
#include "GameAIProg/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Level_CombinedSteering.generated.h"


UCLASS()
class GAMEAIPROG_API ALevel_CombinedSteering : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_CombinedSteering();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

private:
	// Data members
	bool UseMouseTarget = false;
	bool CanDebugRender = false;
	bool CanDebugBehavior = false;
	
	bool IsHoldingLeftButton = false;
	
	
	// Steering Behaviors
	std::unique_ptr<Seek> SeekBehavior{};
	std::unique_ptr<Wander> WanderBehavior{};
	std::vector<std::unique_ptr<EvadeNearby>> EvadeNearbyBehaviors{};	
	// multiple needed bc can't set a different target for an agent with only one steering behavior
	
	std::unique_ptr<BlendedSteering> DrunkBlendedSteering{};
	std::vector<std::unique_ptr<PrioritySteering>> EvadePrioritySteerings{};
	
	UPROPERTY()
	ASteeringAgent* DrunkAgent{};
	
	UPROPERTY()
	TArray<ASteeringAgent*> EvadingAgents{};
	
	TArray<FVector2D> EvadingAgentsPositions{};

	
	void HandleLeftMouseInput(const APlayerController* PlayerController, const FVector& MouseWorldPosition);

	
};

