// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstantHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LevelScriptActor.h"
#include "Templates/Tuple.h"

#include "ImGuiModule.h"
#include "InputMappingContextPriority.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "GameAIProg/Shared/WorldTrimVolume.h"
#include "Shared/ImGuiHelpers.h"

#include "Level_Base.generated.h"

/*
 * Base class for all levels, handles some common variables
 */

UCLASS()
class GAMEAIPROG_API ALevel_Base : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	// Default always active, input mappings (conditionals you must do yourself)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelBase|Input")
	TArray<FInputMappingContextPriority> InputMappingContexts{};
	
	// Sets default values for this actor's properties
	ALevel_Base();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Mouse target render toggle
	bool VisualizeMouseTarget{ false };
	
	AWorldTrimVolume* GetTrimWorld() const { return TrimWorld; }
	
protected:
	// Common UI Vars
	const float MenuWidth{ 235.f };
	FVector2D ViewportSize{};
	ImVec2 WindowSize{};
	ImVec2 WindowPos{};
	
	const FColor MouseTargetColor{ FColor::White };
	const float MouseTargetSize{ ConstantHelpers::DebugDefaultPointSize + 5.f };

	UPROPERTY(EditAnywhere)
	AWorldTrimVolume* TrimWorld{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ASteeringAgent> SteeringAgentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSteeringParams MouseTarget{}; // UHT does not work with using statements, therefore this is called FSteeringParams
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Level input mapping
	UPROPERTY()
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem{nullptr};
	UPROPERTY()
	UEnhancedInputComponent* PlayerEnhancedInputComponent{nullptr};
	UPROPERTY()
	APlayerController* PlayerController{};
	
	bool CanBindLevelInput() const;
	void SetupEnhancedInputAttachment();
	virtual void BindLevelInputMappingContexts();
	virtual void BindLevelInputActions();
	
	// Helpers
	UPROPERTY(EditAnywhere,	BlueprintReadOnly)
	FVector LatestMouseWorldPos{};
	
	std::optional<FVector> GetMouseWorldPos() const;
	void UpdateLatestMouseWorldPos();
};
