// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"

#include "AIController.h"
#include "Components/TextRenderComponent.h"


// Sets default values
ASteeringAgent::ASteeringAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASteeringAgent::BeginPlay()
{
	Super::BeginPlay();
	
	UnrealHelpers::SpawnAndAttachTextToActor(GetWorld(), this);
	
	UActorComponent* ActorComp = GetComponentByClass(UTextRenderComponent::StaticClass());
	if (!ActorComp)
	{
		UE_LOG(LogTemp, Error, TEXT("TextRenderComponent not found"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;
	}
	
	TextRenderComponent = Cast<UTextRenderComponent>(ActorComp);
	if (!TextRenderComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TextRenderComponent couldn't be cast"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;
	}
	
	TextRenderComponent->SetWorldSize(64.f);	
	
	TextRenderComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	
	TextRenderComponent->SetAbsolute(true, true, false);
	TextRenderComponent->SetRelativeRotation(LockedTextRenderRelativeRotation);
	
	TextRenderComponent->SetVisibility(DebugBehaviorTextEnabled);
	
}

void ASteeringAgent::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called every frame
void ASteeringAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (TextRenderComponent)
	{
		const FVector2D RootLocation2D = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		TextRenderComponent->SetWorldLocation(FVector(RootLocation2D, 0.f) + LockedTextRenderRelativeLocation);
	}

	if (SteeringBehavior)
	{
		const SteeringOutput Output = SteeringBehavior->CalculateSteering(DeltaTime, *this);
		if (Output.IsValid)
		{
			AddMovementInput(FVector(Output.LinearVelocity, 0));
            
			if (!IsAutoOrienting())
			{
				if (AAIController* AIController = Cast<AAIController>(GetController()))
				{
					const float DeltaYaw = FMath::Clamp(Output.AngularVelocity, 
						-1.f, 1.f) * GetMaxAngularSpeed() * DeltaTime;
                    
					const FRotator CurrentRotation{GetActorForwardVector().ToOrientationRotator()};
					const FRotator DeltaRotation{0, DeltaYaw, 0};
					const FRotator DesiredRotation{CurrentRotation + DeltaRotation};
                    
					if (!FMath::IsNearlyEqual(DesiredRotation.Yaw, CurrentRotation.Yaw))
					{
						AIController->SetControlRotation(DesiredRotation);
						FaceRotation(DesiredRotation);
					}
				}
			}
		}
	}
	
	
}

// Called to bind functionality to input
void ASteeringAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASteeringAgent::SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior)
{
	if (!NewSteeringBehavior)
	{
		UE_LOG(LogTemp, Error, TEXT("NewSteeringBehavior is nullptr"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;
	}
	
	SteeringBehavior = NewSteeringBehavior;
	
	if (SteeringBehavior->GetClassName() == "Face")
	{
		SetIsAutoOrienting(false);
	}
	else
	{
		SetIsAutoOrienting(true);
	}
	
	if (TextRenderComponent)
	{
		TextRenderComponent->SetText(FText::FromString(SteeringBehavior->GetClassName()));
	}
	
}

void ASteeringAgent::SetDebugBehaviorEnabled(bool DebugBehaviorEnabled)
{
	DebugBehaviorTextEnabled = DebugBehaviorEnabled;
	
	if (!TextRenderComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TextRenderComponent not found"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;
	}
	
	TextRenderComponent->SetVisibility(DebugBehaviorTextEnabled);
}

FText ASteeringAgent::GetDebugBehaviorText() const
{
	if (!TextRenderComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TextRenderComponent not found"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return FText::GetEmpty();
	}
	
	return TextRenderComponent->Text;
}

void ASteeringAgent::SetDebugBehaviorText(const FString& DebugBehaviorText) const
{
	if (!TextRenderComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TextRenderComponent not found"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;	
	}
	
	TextRenderComponent->SetText(FText::FromString(DebugBehaviorText));
}

void ASteeringAgent::SetDebugBehaviorTextColor(const FColor& DebugBehaviorTextColor) const
{
	if (!TextRenderComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TextRenderComponent not found"));
		UnrealHelpers::QuitGameOrPie(GetWorld());
		return;	
	}
	
	TextRenderComponent->SetTextRenderColor(DebugBehaviorTextColor);
}

