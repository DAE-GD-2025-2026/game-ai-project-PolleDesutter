#pragma once

#include <vector>

#include "Shared/Graph/Graph.h"

#include "Heuristics.h"

namespace GameAI
{
	class AStar
	{
	public:
		AStar(Graph* const Graph, HeuristicFunctions::Heuristic HeuristicFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord final
		{
			Node* CurrentNode = nullptr;
			Connection* CurrentConnection = nullptr;
			float CostSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float EstimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& Other) const
			{
				return CurrentNode == Other.CurrentNode
					&& CurrentConnection == Other.CurrentConnection
					&& CostSoFar == Other.CostSoFar
					&& EstimatedTotalCost == Other.EstimatedTotalCost;
			};

			bool operator<(const NodeRecord& Other) const
			{
				return EstimatedTotalCost < Other.EstimatedTotalCost;
			};
		};

		std::vector<Node*> FindPath(Node* const StartNode, const Node* const DestinationNode) const;

	private:
		float GetHeuristicCost(const Node* const StartNode, const Node* const EndNode) const;

		Graph* AStarGraph;
		HeuristicFunctions::Heuristic HeuristicFunction;
		
	};
}
