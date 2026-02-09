#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Shared/ImGuiHelpers.h"

//SEEK
//*******
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	
	// TODO: Add debug rendering
	
	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	Seek TempSeek{};
	TempSeek.SetTarget(Target);
	Steering = TempSeek.CalculateSteering(DeltaT, Agent);
	Steering.LinearVelocity *= -1;
	Steering.AngularVelocity *= -1;
	
	
	// TODO: Add debug rendering
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	const float Direction = Agent.GetRotation();
	const FVector2D Position = Agent.GetPosition();
	UE_LOG(LogTemp, Warning, TEXT("Wandering direction: %f"), Direction);

	const FVector2D CircleCenterOffset{OffsetDistance * FVector2D(FMath::Cos(Direction), FMath::Sin(Direction))};
	const FVector2D CircleCenterPosition = Position + CircleCenterOffset;
	
	WanderAngle = FMath::RandRange(WanderAngle - MaxAngleChange, WanderAngle + MaxAngleChange);

	const FVector2D RandPositionOnCircle = CircleCenterPosition + WanderRadius * FVector2D(cos(WanderAngle), sin(WanderAngle));
	
	Steering.LinearVelocity = RandPositionOnCircle - Agent.GetPosition();
	
	// TODO: Add debug rendering
	if (Agent.GetDebugRenderingEnabled())
	{
		UWorld* World = Agent.GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
			return Steering;
		}
		
		// Debug Steering
		FVector2D DirectionLineEndPoint{ Position + DebugSteeringLineLength * FVector2D(FMath::Cos(Direction), FMath::Sin(Direction)) };
		FVector2D DirectionLineLeftEndPoint{ Position + DebugSteeringLineLength * FVector2D(FMath::Cos(Direction - 90), FMath::Sin(Direction - 90)) };
		
		DrawDebugLine(World, FVector(Position, 0), FVector(DirectionLineEndPoint, 0), DebugSteeringDirectionColor);
		DrawDebugLine(World, FVector(Position, 0), FVector(DirectionLineLeftEndPoint, 0), DebugSteeringDirectionLeftColor);
			
		
		// DrawDebugPoint(World, FVector(DirectionLineEndPoint, 0), 10, DebugCirclePointColor, false, 0.5f);
		
		// Debug Wandering	
		DrawDebugCircle(World, FVector(CircleCenterPosition, 0), WanderRadius, 12, DebugCircleColor, false, 0.f, 0, 2.f, FVector(1,0,0), FVector(0,1,0), false);
		DrawDebugPoint(World, FVector(RandPositionOnCircle, 0), 10, DebugCirclePointColor, false, 0.1f);
		
	}
	
	
	
	
	return Steering;
}

