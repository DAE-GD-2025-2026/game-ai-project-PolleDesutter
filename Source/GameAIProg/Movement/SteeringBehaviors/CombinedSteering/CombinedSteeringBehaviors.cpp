
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"
#include "Compression/lz4.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors) :
	WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
		
	float TotalWeight = 0.0f;
	int NrValidBehaviors = WeightedBehaviors.size();
	
	for (const WeightedBehavior& Behavior : WeightedBehaviors)
	{
		if (!Behavior.pBehavior)
			continue;
	
		if (Behavior.Weight == 0.f)
			continue;
		
		SteeringOutput Output = Behavior.pBehavior->CalculateSteering(DeltaT, Agent);
		
		if (!Output.IsValid)
			continue;
		
		++NrValidBehaviors;
		TotalWeight += Behavior.Weight;
			
		Output *= Behavior.Weight;
			
		BlendedSteering.LinearVelocity += Output.LinearVelocity;
		BlendedSteering.AngularVelocity += Output.AngularVelocity;
		
	}
	
	if (TotalWeight <= 0.f)
		return SteeringOutput();
		
	// Normalize Weights
	BlendedSteering /= TotalWeight;
	
	
	// TODO: Add debug drawing
	if (Agent.GetDebugRenderingEnabled())
	{
		
		
		
	}
	
	

	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	const auto It = 
		std::ranges::find_if(WeightedBehaviors,
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(It != WeightedBehaviors.end())
		return &It->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}