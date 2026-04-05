#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& StartPos, const FVector2D& EndPos,
	NavGraph* const NavigationGraph, std::vector<FVector2D>& DebugNodePositions, std::vector<NavLine>& DebugPortals) 
{
	// TODO: implement
	// Create the path to return
	std::vector<FVector2D> FinalPath{};

	// Get the start and endTriangle

	// We have valid start/end triangles, and they are not the same
	// => Start looking for a path
	// Copy the graph

	// Create Extra node for the Start Node (Agent's position

	// Create extra node for the endNode

	// Run A star on new graph

	// Debug Visualisation

	// Extra: Run optimizer on new graph (First check if everything works without SSFA!)
	// debugPortals = SSFA::FindPortals(nodes, *pNavGraph->GetNavPolygon());
	// finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return FinalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& StartPos, const FVector2D& EndPos, NavGraph* const NavigationGraph)
{
	std::vector<FVector2D> DebugNodePositions{};
	std::vector<NavLine> DebugPortals{};

	return FindPath(StartPos, EndPos, NavigationGraph, DebugNodePositions, DebugPortals);
}