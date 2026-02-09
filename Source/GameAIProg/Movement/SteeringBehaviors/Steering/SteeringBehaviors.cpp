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

	const float Direction = Agent.GetAngularVelocity();
	const FVector2D Position = Agent.GetPosition();

	const FVector2D CircleCenterOffset{OffsetDistance * FMath::Cos(Direction), OffsetDistance * FMath::Sin(Direction)};
	const FVector2D CircleCenterPosition = Position + CircleCenterOffset;
	
	WanderAngle = FMath::RandRange(WanderAngle - MaxAngleChange, WanderAngle + MaxAngleChange);

	const FVector2D RandPositionOnCircle = CircleCenterPosition + WanderRadius * FVector2D(cos(WanderAngle), sin(WanderAngle));
	
	Steering.LinearVelocity = RandPositionOnCircle - Agent.GetPosition();
	
	// TODO: Add debug rendering
	
	
	
	
	return Steering;
}

