#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	if (!pFlock)
		return SteeringOutput{};
		
	Target.Position = pFlock->GetAverageNeighborPos();
	
	return Seek::CalculateSteering(deltaT, pAgent);
}


//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering;
		
	if (!pFlock)
	{
		return SteeringOutput{};
	}

	const FVector2D AgentPosition = pAgent.GetPosition();
	
	auto Neighbors = pFlock->GetNeighbors();
	for (int i = 0; i < pFlock->GetNrOfNeighbors(); ++i)
	{
		const float Distance = FVector2D::Distance(AgentPosition, Neighbors[i]->GetPosition());
		const FVector2D AwayFromTarget = FVector2D(AgentPosition - Neighbors[i]->GetPosition()).GetSafeNormal();
		
		Steering.LinearVelocity += AwayFromTarget / (Distance * Distance);
	}
	
	
	return Seek::CalculateSteering(deltaT, pAgent);
}


//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	if (!pFlock)
		return SteeringOutput{};
	
	SteeringOutput Steering{};
	Steering.LinearVelocity = pFlock->GetAverageNeighborVelocity();
	
	return Steering;
}
