#include "SteeringBehaviors.h"

#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Shared/ImGuiHelpers.h"
#include "Shared/MathHelpers.h"

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
	const FVector2D Position = Agent.GetPosition();

	const FVector2D CircleCenterOffset = OffsetDistance * Direction;
	const FVector2D CircleCenterPosition = Position + CircleCenterOffset;
	
	WanderAngle = FMath::RandRange(Rotation - MaxAngleChange, Rotation + MaxAngleChange);

	const FVector2D RandPositionOnCircle = CircleCenterPosition + WanderRadius * FVector2D(cos(WanderAngle), sin(WanderAngle));
	
	// Seek behavior to the target
	Target.Position = RandPositionOnCircle;
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	
	// Debug Rendering
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

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	const float DistanceToTarget = FVector2D::Distance(Agent.GetPosition(), Target.Position);
	
	// if outside circles, go full speed
	if (DistanceToTarget > SlowRadius)
	{
		Steering.LinearVelocity =  FVector2D(Target.Position - Agent.GetPosition()).GetSafeNormal();
	}
	// if inside target circle, stop
	else if (DistanceToTarget < TargetRadius)
	{
		Steering.LinearVelocity = FVector2D();		
	}
	// if inside slow circle but not in target circle
	else
	{
		float DistancePercentage = (DistanceToTarget - TargetRadius) / (SlowRadius - TargetRadius);
		DistancePercentage = FMath::Clamp(DistancePercentage, 0.0f, 1.0f);
		
		float SpeedMulti = DistancePercentage * MathHelpers::EaseInCubic(DistancePercentage);
		SpeedMulti = FMath::Clamp(SpeedMulti, 0.025f, 1.0f);
		
		Steering.LinearVelocity = FVector2D(Target.Position - Agent.GetPosition()).GetSafeNormal();
		Steering.LinearVelocity *= SpeedMulti * SpeedMulti * Agent.GetMaxLinearSpeed();
		
	}
	
	
	// Debug Rendering
	if (Agent.GetDebugRenderingEnabled())
	{
		const UWorld* World = Agent.GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
			return Steering;
		}	
		
		const FVector2D Position = Agent.GetPosition();
		
		// Debug Arrive	
		DrawDebugCircle(World, FVector(Position, 0), SlowRadius,   12, DebugSlowCircleColor,   false, 0.f, 0, 2.f, FVector(1,0,0), FVector(0,1,0), false);
		DrawDebugCircle(World, FVector(Position, 0), TargetRadius, 12, DebugTargetCircleColor, false, 0.f, 0, 2.f, FVector(1,0,0), FVector(0,1,0), false);
		
		// Debug Target
		DrawDebugPoint(World, FVector(Target.Position, 0), 10, DebugTargetColor, false, 0.1f);
		
	}
	
	
	return Steering;	
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};	
	
	
	const float DistanceToTarget = FVector2D::Distance(Agent.GetPosition(), Target.Position);
	const float AgentSpeed = Agent.GetMaxLinearSpeed();
	
	if (AgentSpeed == 0.f)
	{
		return Steering;
	}
	
	const float TimeToReachTarget = DistanceToTarget / AgentSpeed; 
	
	PredictedPosition = Target.Position + Target.LinearVelocity * TimeToReachTarget;
	
	Steering.LinearVelocity = (PredictedPosition - Agent.GetPosition()).GetSafeNormal();
	
	
	// Check if predicted position is 
	
		
	const FString Message = FString::Printf(TEXT("PositionTarget: %s, TimeToReach: %f"), *PredictedPosition.ToString(), TimeToReachTarget);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, Message);
	
	// Debug Rendering
	if (Agent.GetDebugRenderingEnabled())
	{
		const UWorld* World = Agent.GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
			return Steering;
		}	
		
		DrawDebugPoint(World, FVector(PredictedPosition, 0.f), 100.f, FColor::Red);
	}
	
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	
	// Toggle Debug rendering off for temp result
	const bool DebugEnabled = Agent.GetDebugRenderingEnabled();
	Agent.SetDebugRenderingEnabled(false);
	
	// Set Steering
	Pursuit TempPursuit{};
	TempPursuit.SetTarget(Target);
	Steering = TempPursuit.CalculateSteering(DeltaT, Agent);
	
	// Toggle Debug rendering to previous value
	Agent.SetDebugRenderingEnabled(DebugEnabled);
	
	// Go the opposite direction of the pursuit direction
	Steering.LinearVelocity *= -1;
	

	if (Agent.GetDebugRenderingEnabled())
	{
		const UWorld* World = Agent.GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
			return Steering;
		}	
		
		const FVector2D PredictedPosition = TempPursuit.GetPredictedPosition();
		const FVector2D EvadePosition = Agent.GetPosition() - (PredictedPosition - Agent.GetPosition());	
		
		DrawDebugPoint(World, FVector(PredictedPosition, 0.f), 100.f, DebugPredictedLocationColor);
		DrawDebugPoint(World, FVector(EvadePosition, 0.f), 100.f, DebugEvadingLocationColor);
				
	}
	
	return Steering;
}

