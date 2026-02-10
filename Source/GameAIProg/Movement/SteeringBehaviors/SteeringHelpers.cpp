#include "SteeringHelpers.h"

#include "SteeringAgent.h"
#include "Shared/ConstantHelpers.h"

void SteeringHelpers::DrawDebugDirection(const ASteeringAgent& Agent)
{
	const UWorld* World = Agent.GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
		return;
	}
	
	const FVector2D AgentPosition = Agent.GetPosition();
	const FVector2D Direction = Agent.GetLinearVelocity().GetSafeNormal();
		
	// Debug Steering
	const FVector2D DirectionEndPoint{ AgentPosition + ConstantHelpers::DebugDefaultLineLength * Direction };
	const FVector2D DirectionLeftEndPoint{ AgentPosition + ConstantHelpers::DebugDefaultLineLength * FVector2D(Direction.Y, -Direction.X) };
		
	DrawDebugLine(World, FVector(AgentPosition, 0), FVector(DirectionEndPoint, 0), ConstantHelpers::DebugDirectionColor);
	DrawDebugLine(World, FVector(AgentPosition, 0), FVector(DirectionLeftEndPoint, 0), ConstantHelpers::DebugDirectionLeftColor);
				
}
