#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>

class ASteeringAgent;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) = 0;

	void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }
	
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	FTargetData Target;
};

// Your own SteeringBehaviors should follow here...

class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) override;
	
};

class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() = default;
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) override;
	
protected:
	const FColor DebugFleePointColor{ FColor::Blue };
	
};

class Wander : public ISteeringBehavior
{
public:
	Wander() = default;
	virtual ~Wander() = default;
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) override;
	
	void SetWanderOffset(float Offset) { OffsetDistance = Offset; }
	void SetWanderRadius(float Radius) { WanderRadius = Radius; }
	// Angle in degrees
	void SetMaxAngleChange(float Angle) { MaxAngleChange = Angle; }
	
protected:
	float OffsetDistance{ 200.f };
	float WanderRadius{ 200.f };
	float MaxAngleChange{ 45.f };
	float WanderAngle{ 0.f };
	
	const FColor DebugCircleColor{ FColor::Blue };	
	const FColor DebugCirclePointColor{ FColor::Green };	
	
};

class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) override;
	
protected:
	float SlowRadius{ 700.f };
	float TargetRadius{ 200.f };
	
	const FColor DebugSlowCircleColor{ FColor::Blue };	
	const FColor DebugTargetCircleColor{ FColor::Orange };	
	
};

class Pursuit : public ISteeringBehavior
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) override;
	
	FVector2D GetPredictedPosition() const { return PredictedPosition; }
	
protected:
	FVector2D PredictedPosition;
	
	const FColor DebugTargetCurrentLocationColor{ FColor::Blue };
	
};

class Evade : public ISteeringBehavior
{
public:
	Evade() = default;
	virtual ~Evade() = default;
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) override;
	
protected:
	const FColor DebugPredictedLocationColor{ FColor::Blue };
	
};

