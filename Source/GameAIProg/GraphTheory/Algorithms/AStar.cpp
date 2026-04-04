#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const Graph, HeuristicFunctions::Heuristic Function)
	: AStarGraph(Graph)
	, HeuristicFunction(Function)
{
}

std::vector<Node*>AStar::FindPath(Node* const StartNode, Node* const GoalNode)
{
	// TODO: Add algorithm
	std::vector<Node*> path{};
	return path;
}

float AStar::GetHeuristicCost(Node* const StartNode, Node* const EndNode) const
{
	const FVector2D ToDestination = AStarGraph->GetNode(EndNode->GetId())->GetPosition() - AStarGraph->GetNode(StartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(ToDestination.X), abs(ToDestination.Y));
}