#include "Shared/MathHelpers.h"

#include "Kismet/KismetMathLibrary.h"

float MathHelpers::NormalizeAngleDegrees(float Angle)
{
	return UKismetMathLibrary::NormalizeAxis(Angle);
}

float MathHelpers::NormalizeAngleRadians(float Angle)
{
	return FMath::DegreesToRadians(UKismetMathLibrary::NormalizeAxis(FMath::RadiansToDegrees(Angle)));
}
