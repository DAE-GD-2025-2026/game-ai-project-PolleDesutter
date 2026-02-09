#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

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

	// TODO: Add debug rendering
	return Steering;
}

