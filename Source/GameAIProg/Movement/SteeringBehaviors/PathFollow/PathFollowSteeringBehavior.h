#pragma once

#include <vector>

#include "../Steering/SteeringBehaviors.h"

class PathFollow : public ISteeringBehavior
{
public:
	PathFollow();
	virtual ~PathFollow() override;
	void SetPath(const std::vector<FVector2D>& Path);
	virtual SteeringOutput CalculateSteering(float DeltaTime, ASteeringAgent & Agent) override;
	virtual FString GetClassName() override { return CLASS_NAME_FSTRING; }

private:
	Seek* pSeek = nullptr;
	Arrive* pArrive = nullptr;
	ISteeringBehavior* pCurrentSteering = nullptr;
	std::vector<FVector2D> PathVec = {};
	int CurrentPathIndex = 0;

	void GotoNextPathPoint();
};

