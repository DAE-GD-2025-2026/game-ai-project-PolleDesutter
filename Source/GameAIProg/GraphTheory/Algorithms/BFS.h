#pragma once
#include <map>
#include <vector>

namespace GameAI
{
	class Graph;
	class Node;

	class BFS
	{
	public:
		BFS(Graph* const Graph);

		std::vector<Node*> FindPath(Node* const StartNode, Node* const DestinationNode) const;

	private:
		std::vector<Node*> ReconstructPath(const std::map<int, int>& ParentMap, Node* const StartNode, Node* const GoalNode) const;
		std::vector<int> GetNeighborsIds(int NodeId) const;
		
		
		Graph* BfsGraph;
	};
}
