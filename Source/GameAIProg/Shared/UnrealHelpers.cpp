#include "UnrealHelpers.h"


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
	}
	
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr"));
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

