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

	if (pFlock->DebugRenderAveragePosition)
	{
		DrawDebugPoint(pAgent.GetWorld(), FVector(Target.Position, 0), 
			ConstantHelpers::DebugDefaultPointSize, FColor::Yellow);
	}
	
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
	
	const auto Neighbors = pFlock->GetNeighbors();
	for (int i = 0; i < pFlock->GetNrOfNeighbors(); ++i)
	{ 
		const FVector2D NeighborPosition = Neighbors[i]->GetPosition();
		const float Distance = FVector2D::Distance(AgentPosition, NeighborPosition);
		const FVector2D AwayFromTarget = FVector2D(AgentPosition - NeighborPosition).GetSafeNormal();
		
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
