#include "UnrealHelpers.h"


bool UnrealHelpers::IsPositionInsideVolume(const UBoxComponent& BoxComponent, const FVector& Position)
{
	const FVector BoxExtents = BoxComponent.GetUnscaledBoxExtent();
	const FBox Box(-BoxExtents, BoxExtents);
	
	return Box.IsInside(Position);
}

