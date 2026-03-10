#include "PathFollowSteeringBehavior.h"
#include "../SteeringAgent.h"

PathFollow::PathFollow()
{
	pSeek = new Seek();
	pArrive = new Arrive();
	pArrive->SetTargetRadius(10.0f);
}

PathFollow::~PathFollow()
{
	delete pArrive;
	delete pSeek;
}

void PathFollow::SetPath(const std::vector<FVector2D>& Path)
{
	PathVec = Path;  
	
	CurrentPathIndex = -1;
	GotoNextPathPoint();
}

SteeringOutput PathFollow::CalculateSteering(float DeltaTime, ASteeringAgent& Agent)
{
	if (CurrentPathIndex < static_cast<int>(PathVec.size()))
	{
		const float AgentRadius = Agent.GetCapsuleRadius();
		const FVector2D ToPathPoint{ PathVec[CurrentPathIndex] - Agent.GetPosition() };
		
		if (ToPathPoint.SizeSquared() < AgentRadius * AgentRadius)
		{
			// Reached point of the path
			GotoNextPathPoint();
		}
	}

	if (pCurrentSteering != nullptr)
	{
		return pCurrentSteering->CalculateSteering(DeltaTime, Agent);
	}
	return SteeringOutput{};
}

void PathFollow::GotoNextPathPoint()
{
	++CurrentPathIndex;
	if (CurrentPathIndex >= static_cast<int>(PathVec.size())) return;
	
	if (CurrentPathIndex == PathVec.size() -1)
	{
		const FTargetData PathTarget { PathVec[CurrentPathIndex] };
		
		// We have reached the last node
		pArrive->SetTarget(PathTarget);
		pCurrentSteering = pArrive;
	}
	else
	{
		const FTargetData PathTarget{ PathVec[CurrentPathIndex] };
		// Move to the next node
		pSeek->SetTarget(PathTarget);
		pCurrentSteering = pSeek;
	}
}

