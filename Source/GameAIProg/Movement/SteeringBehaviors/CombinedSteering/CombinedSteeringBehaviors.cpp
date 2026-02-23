#include "CombinedSteeringBehaviors.h"
#include <algorithm>

#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Shared/ConversionHelpers.h"


//****************
//BLENDED STEERING
BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors) :
	WeightedBehaviors(WeightedBehaviors)
{};


SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	Agent.SetIsAutoOrienting(true);

	SteeringOutput BlendedSteering = {};
		
	float TotalWeight = 0.0f;
	int NrValidBehaviors = WeightedBehaviors.size();
	
	for (const WeightedBehavior& Behavior : WeightedBehaviors)
	{
		if (!Behavior.pBehavior)
		{
			continue;
		}
	
		if (Behavior.Weight == 0.f)
		{
			continue;
		}
		
		SteeringOutput Output = Behavior.pBehavior->CalculateSteering(DeltaT, Agent);
		
		if (!Output.IsValid)
		{
			continue;
		}
		
		++NrValidBehaviors;
		TotalWeight += Behavior.Weight;
			
		Output *= Behavior.Weight;
			
		BlendedSteering.LinearVelocity += Output.LinearVelocity;
		BlendedSteering.AngularVelocity += Output.AngularVelocity;
		
	}
	
	if (TotalWeight <= 0.f)
	{
		return SteeringOutput();
	}
		
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

	if(It == WeightedBehaviors.end())
	{
		return nullptr;
	}
	
	return &It->Weight;
}


//*****************
//PRIORITY STEERING
PrioritySteering::PrioritySteering(const std::vector<ISteeringBehavior*>& PrioritySteeringBehaviors) :
	PriorityBehaviors(PrioritySteeringBehaviors)
{
	if (!PriorityBehaviors.empty())
	{
		CurrentBehavior = PriorityBehaviors.front();
		
		const FString BehaviorClassName = CurrentBehavior->GetClassName();
		UE_LOGFMT(LogTemp, Warning, "BehaviorClassName: {Name}", BehaviorClassName);
	}
	
}

SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	Agent.SetIsAutoOrienting(true);
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const Behavior : PriorityBehaviors)
	{
		Steering = Behavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
		{
			break;
		}
	}
	
	return Steering;
}