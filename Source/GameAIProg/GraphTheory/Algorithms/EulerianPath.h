#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected())
		{
			return Eulerianity::notEulerian;
		}

		// TODO Count nodes with odd degree 
		const auto& Nodes = m_pGraph->GetNodes();
		const int OddNodesCount = std::ranges::count_if(Nodes, 
			[&](const auto& Node)
			{
				const auto Connections = m_pGraph->FindConnectionsFrom(Node->GetId());
				return (Connections.size() % 2) == 1;
			});
	
		
		// TODO A connected graph with no odd nodes is Eulerian
		if (OddNodesCount == 0)
		{
			return Eulerianity::eulerian;
		}
		
		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (OddNodesCount > 2)
		{
			return Eulerianity::notEulerian;
		}
		
		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes
		if (OddNodesCount == 2 && Nodes.size() != 2)
		{
			return Eulerianity::semiEulerian;
		}
		
		
		// What about OddNodesCount == 1??
		return Eulerianity::notEulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph GraphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = GraphCopy.GetActiveNodes();
		int CurrentNodeId{ Graphs::InvalidNodeId };
		
		
		if (Nodes.empty())
		{
			UE_LOG(LogTemp, Warning, TEXT("FindPath: Nodes is empty"));
			return Path;
		}
		
		
		// TODO Check if there can be an Euler path
		if (eulerianity == Eulerianity::eulerian)
		{
			CurrentNodeId = Nodes[0]->GetId();
		}
		else if (eulerianity == Eulerianity::semiEulerian)
		{
			// Choose node with odd degree		
			auto OddDegreeNodeIt = std::ranges::find_if(Nodes, 
				[this](const auto Node)
				{
					const auto Connections = m_pGraph->FindConnectionsFrom(Node->GetId());
					
					const bool IsOddDegree = (Connections.size() % 2) == 1;
					return IsOddDegree;
				});
		
			if (OddDegreeNodeIt == Nodes.end())
			{
				UE_LOG(LogTemp, Warning, TEXT("FindPath: No Odd Degree Node Found, but there normally should be one"));
				return Path;
			}
			
			CurrentNodeId = (*OddDegreeNodeIt)->GetId();
			
		}
		else if (eulerianity == Eulerianity::notEulerian)
		{
			// TODO If this graph is not eulerian, return the empty path
			return Path;
		}
		
	
		// TODO Start algorithm loop
		std::stack<int> NodeIdStack;
		
		
		// https://web.archive.org/web/20240920214900/https://www.graph-magics.com/articles/euler.php
		while (true)
		{
			if (CurrentNodeId == Graphs::InvalidNodeId)
			{
				UE_LOG(LogTemp, Warning, TEXT("FindPath: Algorithm Loop, CurrentNodeId is invalid"));
				return {};
			}
			
			// Break the repeating, if the current node has no more connections AND the stack is empty 
			const auto Connections = GraphCopy.FindConnectionsFrom(CurrentNodeId);
			if(Connections.empty() && NodeIdStack.empty())
			{
				break;
			}
			
			// if the connections are empty
			// Add the stack's NodeId's as Path Nodes
			// This is traversing the list from back to front
			if (Connections.empty())
			{
				// set currentNode as path, and go back
				Path.push_back(m_pGraph->GetNode(CurrentNodeId).get());
				CurrentNodeId = NodeIdStack.top();
				
				continue;
			}
				
			// Add the node to the stack	
			NodeIdStack.push(CurrentNodeId);
			
			// Take any of it's neighbors
			// I take the first neighbor, because ease of use
			const auto NeighborId = Connections[0]->GetToId();
			
			// Remove the connection between the current node and the neighbor
			GraphCopy.RemoveConnection(CurrentNodeId, NeighborId);
			
			// Set the neighbor as the current vertex
			CurrentNodeId = NeighborId;
				
		}
	
		// Because the path is in reverse direction, reverse the path
		std::ranges::reverse(Path);
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		if (startIndex < 0 || startIndex >= static_cast<int>(Nodes.size()))
		{
			UE_LOG(LogTemp, Warning, TEXT("VisitedAllNodesDFS: Startindex out of bounds"));
			return;					
		}
		
		visited[startIndex] = true;
		
		// TODO Ask the graph for the connections from that node
		const auto Connections = m_pGraph->FindConnectionsFrom(startIndex);
		
		// TODO recursively visit any valid connected nodes that were not visited before
		// TODO Tip: use an index-based for-loop to find the correct index
		for (int i = 0; i < Connections.size(); ++i)
		{
			const auto Connection = Connections[i];
			const auto ConnectionToId = Connection->GetToId();
			
			for (int j = 0; j < Nodes.size(); ++j)
			{
				if (ConnectionToId != Nodes[j]->GetId())
				{
					continue;
				}
				
				if (visited[j] == false)
				{
					VisitAllNodesDFS(Nodes, visited, ConnectionToId);
				}
				
			}
			
		}
		
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.empty())
		{
			return false;
		}
		std::vector<bool> HasNodeBeenVisitedList(Nodes.size(), false);

		// TODO choose a starting node
		int StartingNodeIdx = Graphs::InvalidNodeId;
		for (const auto Node : Nodes)
		{
			const auto Connections = m_pGraph->FindConnectionsFrom(StartingNodeIdx);	
			if (Connections.empty())
			{
				continue;
			}
			
			StartingNodeIdx = Node->GetId();
			if (StartingNodeIdx == Graphs::InvalidNodeId)
			{
				continue;
			}
			
			break;
		}
		
		if (StartingNodeIdx == Graphs::InvalidNodeId)
		{
			return false;
		}
		
		// TODO start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, HasNodeBeenVisitedList,  StartingNodeIdx);
		
		// TODO if a node was never visited, this graph is not connected
		const bool HaveAllNodesBeenVisitedList = std::ranges::all_of(HasNodeBeenVisitedList, 
			[](const bool HasBeenVisited)
             {
                 return HasBeenVisited;							
             });
	
		return HaveAllNodesBeenVisitedList;
	}
}