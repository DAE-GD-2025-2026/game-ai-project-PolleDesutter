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

	const float Rotation = Agent.GetRotation();
	const FVector2D Direction = Agent.GetLinearVelocity().GetSafeNormal();
	// const float Direction{};
	const FVector2D Position = Agent.GetPosition();

	const FVector2D CircleCenterOffset = OffsetDistance * Direction;
	const FVector2D CircleCenterPosition = Position + CircleCenterOffset;
	
	WanderAngle = FMath::RandRange(Rotation - MaxAngleChange, Rotation + MaxAngleChange);

	const FVector2D RandPositionOnCircle = CircleCenterPosition + WanderRadius * FVector2D(cos(WanderAngle), sin(WanderAngle));
	
	// Seek behavior to the target
	Target.Position = RandPositionOnCircle;
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	
	// TODO: Add debug rendering
	if (Agent.GetDebugRenderingEnabled())
	{
		const UWorld* World = Agent.GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
			return Steering;
		}
		
		// Debug Steering
		const FVector2D DirectionEndPoint{ Position + DebugSteeringLineLength * Direction };
		const FVector2D DirectionLeftEndPoint{ Position + DebugSteeringLineLength * FVector2D(Direction.Y, -Direction.X) };
		
		DrawDebugLine(World, FVector(Position, 0), FVector(DirectionEndPoint, 0), DebugSteeringDirectionColor);
		DrawDebugLine(World, FVector(Position, 0), FVector(DirectionLeftEndPoint, 0), DebugSteeringDirectionLeftColor);
			
		// Debug Wandering	
		DrawDebugCircle(World, FVector(CircleCenterPosition, 0), WanderRadius, 12, DebugCircleColor, false, 0.f, 0, 2.f, FVector(1,0,0), FVector(0,1,0), false);
		DrawDebugPoint(World, FVector(RandPositionOnCircle, 0), 10, DebugCirclePointColor, false, 0.1f);
		
	}
	
	
	
	
	return Steering;
}

