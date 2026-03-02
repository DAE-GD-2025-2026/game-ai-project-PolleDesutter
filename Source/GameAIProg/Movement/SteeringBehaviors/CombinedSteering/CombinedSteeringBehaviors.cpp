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
	SteeringOutput BlendedSteering = {};
	
	Agent.SetDebugBehaviorTextColor(CurrentBehaviorColor);
		
	float TotalWeight = 0.0f;
	int NrValidBehaviors = WeightedBehaviors.size();
	
	for (const WeightedBehavior& Behavior : WeightedBehaviors)
	{
		if (!Behavior.pBehavior)
		{
			continue;
		}
	
		if (Behavior.Weight <= 0.f)
		{
			continue;
		}
		
		SteeringOutput Output = Behavior.pBehavior->CalculateSteering(DeltaT, Agent);
		Output *= Behavior.Weight;
		
		if (!Output.IsValid)
		{
			continue;
		}
		
		++NrValidBehaviors;
		TotalWeight += Behavior.Weight;
			
		BlendedSteering = BlendedSteering + Output;
	}
	
	if (TotalWeight <= 0.f)
	{
		return SteeringOutput();
	}
		
	// Normalize Weights
	BlendedSteering /= TotalWeight;
	
	
	if (Agent.GetDebugRenderingEnabled())
	{
		const UWorld* World = Agent.GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
			return BlendedSteering;
		}
				
		SteeringHelpers::DrawDebugDirection(Agent);
		
		// Draw Target
		DrawDebugPoint(World, FVector(Target.Position, 0), 
			ConstantHelpers::DebugDefaultPointSize, TargetColor);
		
		
		SteeringHelpers::DrawDebugLineFromDirection(World, FVector(Agent.GetPosition(), 0.f),
			Agent.GetRotation(), ConstantHelpers::DebugDefaultLineLength, 
			FColor::White);
		
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
	}
	
}

SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const Behavior : PriorityBehaviors)
	{
		Steering = Behavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
		{
			// If new Behavior is selected
			if (CurrentBehavior != Behavior)
			{
				CurrentBehavior = Behavior;
				Agent.SetDebugBehaviorText(GetClassName());
				Agent.SetDebugBehaviorTextColor(CurrentBehaviorColor);
			}
			
			break;
		}
	}
	
	return Steering;
}

