// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAIProg/Shared/BaseAgent.h"
#include "Steering/SteeringBehaviors.h"
#include "SteeringAgent.generated.h"

/*
 * Simple agent which will run a steering behavior and move according to its output
 *
 * ┌────────────────────┐                                     
 * │                    │                                     
 * │      ABaseAgent    │                                     
 * │                    │                                     
 * └─────────▲──────────┘                                     
 *           │                                                
 *           │inherits                                        
 * ┌─────────┼──────────┐            ┌───────────────────────┐
 * │                    │   uses     │                       │
 * │   ASteeringAgent   ├────────────►   ISteeringBehavior   │
 * │                    │            │                       │
 * └────────────────────┘            └─┬─────────────────────┘
 *                                     │                      
 *                                     │ inherits   ┌────────┐
 *                                     │            │        │
 *                                     ├───────────►│  Seek  │
 *                                     │            │        │
 *                                     │            └────────┘
 *                                     │                      
 *                                     │            ┌────────┐
 *                                     │            │        │
 *                                     ├───────────►│  Flee  │
 *                                     │            │        │
 *                                     │            └────────┘
 *                                     │                      
 *                                     │            ┌────────┐
 *                                     │            │        │
 *                                     └───────────►│  ....  │
 *                                                  │        │
 *                                                  └────────┘
 * 
 */

class UTextRenderComponent;

UCLASS()
class GAMEAIPROG_API ASteeringAgent : public ABaseAgent
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASteeringAgent();

protected:

	ISteeringBehavior* SteeringBehavior{ nullptr }; // non-owning
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the object is being destroyed
	virtual void BeginDestroy() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior);

	UMaterialInterface* GetHighlightedMaterial() const { return HighlightedBodyMaterial; }
	UMaterialInterface* GetNormalMaterial() const { return NormalBodyMaterial; }
	
	bool GetDebugBehaviorText() const{ return DebugBehaviorTextEnabled; };
	void SetDebugBehaviorText(bool DebugBehaviorText);
	
	
protected:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true, Category="Materials"))
	UMaterialInterface* HighlightedBodyMaterial;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true, Category="Materials"))
	UMaterialInterface* NormalBodyMaterial;
	
	UPROPERTY()
	UTextRenderComponent* TextRenderComponent;
	
	const FRotator	LockedTextRenderRelativeRotation{90.f, 180.f, 0.f};
	const FVector	LockedTextRenderRelativeLocation{80.f, 0.f, 20.f};
	const float		LockedTextRenderFontSize{ 32.f };
	
	bool DebugBehaviorTextEnabled{ false };
	
};
