#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& StartPos, const FVector2D& EndPos,
	const NavGraph* const NavigationGraph, std::vector<FVector2D>& DebugNodePositions, std::vector<NavLine>& DebugPortals)
{
	// Create the path to return
	std::vector<FVector2D> FinalPath{};
	const TriPolygon* NavigationPolygon = NavigationGraph->GetNavPolygon();

	if (!NavigationPolygon)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavMeshPathfinding::FindPath: Graph has not NavPoly"));	
		return std::vector<FVector2D>{};
	}
	
	// Get the start and endTriangle
	const TriPolygon::Triangle* StartTriangle = NavigationPolygon->GetTriangleAtPosition(StartPos, false);
	const TriPolygon::Triangle* EndTriangle = NavigationPolygon->GetTriangleAtPosition(EndPos, false);

	if (StartTriangle == nullptr || EndTriangle == nullptr)
	{
		return std::vector<FVector2D>();
	}
	
	if (StartTriangle == EndTriangle)
	{
		FinalPath.push_back(StartPos);	
		FinalPath.push_back(EndPos);	
		return FinalPath;
	}

	// We have valid start/end triangles, and they are not the same
	// => Start looking for a path
	// Copy the graph
	const std::unique_ptr<NavGraph> GraphCopy = NavigationGraph->Clone();
	
	// Create Extra node for the Start Node (Agent's position)
	const int StartNodeIdx = GraphCopy->AddNode(std::make_unique<NavGraphNode>(StartPos, -1));
	
	// Create extra node for the endNode
	const int EndNodeIdx = GraphCopy->AddNode(std::make_unique<NavGraphNode>(EndPos, -1));

	// this should be a function or lamba
	
	// Connect StartNode to StartTriangle
	for (const TriPolygon::Edge& Edge : StartTriangle->GetEdges())
	{
		std::optional<int> EdgeIdxOptional = NavigationPolygon->FindEdgeIndex(Edge);
		if (EdgeIdxOptional.has_value())
		{
			const int EdgeIdx = EdgeIdxOptional.value();
			if (EdgeIdx == Graphs::InvalidNodeId)
			{
				continue;
			}
			
			const int NodeIdx = GraphCopy->GetNodeIdFromEdgeIndex(EdgeIdx);
			if (NodeIdx == Graphs::InvalidNodeId)
			{
				continue;
			}
			
			auto NodeConnection = std::make_unique<Connection>(NodeIdx, StartNodeIdx);
			GraphCopy->AddConnection(std::move(NodeConnection));
			
		}
	}

	// Connect EndNode to EndTriangle
	for (const TriPolygon::Edge& Edge : EndTriangle->GetEdges())
	{
		std::optional<int> EdgeIdxOptional = NavigationPolygon->FindEdgeIndex(Edge);
		if (EdgeIdxOptional.has_value())
		{
			const int EdgeIdx = EdgeIdxOptional.value();
			if (EdgeIdx == Graphs::InvalidNodeId)
			{
				continue;
			}
			
			const int NodeIdx = GraphCopy->GetNodeIdFromEdgeIndex(EdgeIdx);
			if (NodeIdx == Graphs::InvalidNodeId)
			{
				continue;
			}
			
			auto NodeConnection = std::make_unique<Connection>(NodeIdx, EndNodeIdx);
			GraphCopy->AddConnection(std::move(NodeConnection));
			
		}
	}

	// Run A star on new graph
	const AStar AStarAlgorithm{ GraphCopy.get(), HeuristicFunctions::Chebyshev }; 
	const std::vector<Node*> PathNodes = AStarAlgorithm.FindPath(GraphCopy->GetNode(StartNodeIdx).get(), GraphCopy->GetNode(EndNodeIdx).get());
	for (const Node* PathNode : PathNodes)
	{
		if (PathNode == nullptr)
		{
			break;
		}
		
		FinalPath.push_back(PathNode->GetPosition());
	}
	

	// Debug Visualisation

	// Extra: Run optimizer on new graph (First check if everything works without SSFA!)
	// debugPortals = SSFA::FindPortals(nodes, *pNavGraph->GetNavPolygon());
	// finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return FinalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& StartPos, const FVector2D& EndPos, 
	const NavGraph* const NavigationGraph)
{
	std::vector<FVector2D> DebugNodePositions{};
	std::vector<NavLine> DebugPortals{};

	return FindPath(StartPos, EndPos, NavigationGraph, DebugNodePositions, DebugPortals);
}