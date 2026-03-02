#include "UnrealHelpers.h"

#include "Components/TextRenderComponent.h"

#if WITH_EDITOR
	#include "Editor.h"
	#include "UnrealEdGlobals.h"
	#include "Editor/UnrealEdEngine.h"
#endif

bool UnrealHelpers::IsPositionInsideVolume(const UBoxComponent& BoxComponent, const FVector& Position)
{
	const FVector BoxExtents = BoxComponent.GetUnscaledBoxExtent();
	const FBox Box(-BoxExtents, BoxExtents);
	
	return Box.IsInside(Position);
}


bool UnrealHelpers::GetMouseWorldPosition(const UObject* WorldContextObject, const APlayerController* PlayerController, FVector& OutPosition)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldContextObject is nullptr"));
		UnrealHelpers::QuitGameOrPie(WorldContextObject->GetWorld());
	}
	
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr"));
		UnrealHelpers::QuitGameOrPie(WorldContextObject->GetWorld());
	}
	
	
	FVector2D MousePos{};
	PlayerController->GetMousePosition(MousePos.X, MousePos.Y);
		
	FVector WorldOrigin{};
	FVector WorldDirection{};
		
	PlayerController->DeprojectScreenPositionToWorld(MousePos.X, MousePos.Y, WorldOrigin, WorldDirection);
		
	const FVector TraceStart = WorldOrigin;
	const FVector TraceEnd = WorldOrigin + 10000.f * WorldDirection;
		
	FHitResult HitResult{};
	const bool HasHit = WorldContextObject->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, 
		ECC_Visibility);
		
	if (HasHit)
	{
		OutPosition = HitResult.Location;	
	}

	return HasHit;
			
}

void UnrealHelpers::SpawnAndAttachTextToActor(const UObject* WorldContextObject, AActor* Actor)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldContextObject is nullptr"));
		return;
	}
	
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor is nullptr"));
		UnrealHelpers::QuitGameOrPie(WorldContextObject->GetWorld());
		return;
	}
	
	auto TextRenderComp = NewObject<UTextRenderComponent>(Actor);	
	if (!TextRenderComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextRenderComp is nullptr"));
		UnrealHelpers::QuitGameOrPie(WorldContextObject->GetWorld());
		return;
	}
	
	// Attach to root of actor
	TextRenderComp->SetupAttachment(Actor->GetRootComponent());
	
	// Register component with actor
	TextRenderComp->RegisterComponent();
	
	// Set default text
	TextRenderComp->SetText(INVTEXT("Default"));
	
}

void UnrealHelpers::QuitGameOrPie(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("QuitGameOrPie: WorldContextObject is nullptr"));
		return;
	}
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("QuitGameOrPie: World is nullptr"));
		return;
	}
	
#if WITH_EDITOR
	if (GEditor && GEditor->PlayWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("QuitGameOrPie: PIE quit"));
		GEditor->RequestEndPlayMap();
		return;
	}
#endif
	
	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("QuitGameOrPie: PlayerController is nullptr"));
		return;
	}
	
	UKismetSystemLibrary::QuitGame(WorldContextObject, PlayerController, 
		EQuitPreference::Quit, false);
	
}


