#pragma once
#include <vector>

#include "../Steering/SteeringBehaviors.h"

//****************
//BLENDED STEERING
class BlendedSteering final: public ISteeringBehavior
{
public:
	struct WeightedBehavior
	{
		ISteeringBehavior* pBehavior = nullptr;
		float Weight = 0.f;

		WeightedBehavior(ISteeringBehavior* const pBehavior, float Weight) :
			pBehavior(pBehavior),
			Weight(Weight)
		{};
	};

	BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors);

	void AddBehaviour(const WeightedBehavior& WeightedBehavior) { WeightedBehaviors.push_back(WeightedBehavior); }
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	virtual FString GetClassName() override { return CLASS_NAME_FSTRING; }

	float* GetWeight(ISteeringBehavior* const SteeringBehavior);
	
	// returns a reference to the weighted behaviors, can be used to adjust weighting. Is not intended to alter the behaviors themselves.
	std::vector<WeightedBehavior>& GetWeightedBehaviorsRef() { return WeightedBehaviors; }

private:
	std::vector<WeightedBehavior> WeightedBehaviors = {};

	// using ISteeringBehavior::SetTarget; // made private because targets need to be set on the individual behaviors, not the combined behavior
};

//*****************
//PRIORITY STEERING
class PrioritySteering final: public ISteeringBehavior
{
public:
	PrioritySteering(const std::vector<ISteeringBehavior*>& PrioritySteeringBehaviors);

	void AddBehaviour(ISteeringBehavior* const Behavior) { PriorityBehaviors.push_back(Behavior); }
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	virtual FString GetClassName() override { return CLASS_NAME_FSTRING; }

	
	bool CanPrintCurrentBehavior{ false };	
	
private:
	std::vector<ISteeringBehavior*> PriorityBehaviors = {};
	
	ISteeringBehavior* CurrentBehavior = nullptr;
	

	// using ISteeringBehavior::SetTarget; // made private because targets need to be set on the individual behaviors, not the combined behavior
};