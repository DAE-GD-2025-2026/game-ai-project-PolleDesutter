#include "BFS.h"

#include <list>
#include <map>
#include <queue>

#include "IndexTypes.h"
#include "Shared/ConversionHelpers.h"
#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const Graph)
	: BfsGraph{ Graph }
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const StartNode, Node* const DestinationNode) const
{
	if (!StartNode)
	{
		UE_LOG(LogTemp, Error, TEXT("StartNode is invalid"));
		return std::vector<Node*>();
	}
	if (!DestinationNode)
	{
		UE_LOG(LogTemp, Error, TEXT("DestinationNode is invalid"));
		return std::vector<Node*>();
	}
	
	if (StartNode->GetId() == Graphs::InvalidNodeId)
	{
		UE_LOG(LogTemp, Error, TEXT("StartNode Id is invalid"));
		return std::vector<Node*>();
	}
	if (DestinationNode->GetId() == Graphs::InvalidNodeId)
	{
		UE_LOG(LogTemp, Error, TEXT("DestinationNode Id is invalid"));
		return std::vector<Node*>();
	}
	
	
	// Start Algorithm
	
	// Get Neighbors
	const auto& ToConnections = BfsGraph->FindConnectionsFrom(StartNode->GetId());
	if (ToConnections.empty())
	{
		UE_LOG(LogTemp, Error, TEXT("BFS::FindPath: No connections found"));
		return std::vector<Node*>();
	}
	
	std::vector<int> NeighborIds(ToConnections.size(), IndexConstants::InvalidID);
	for (int i = 0; i < ToConnections.size(); ++i)
	{
		if (!ToConnections[i])
		{
			continue;
		}
		
		NeighborIds[i] = ToConnections[i]->GetToId();
	}
	
	
	std::queue<int> QueueNodeIds{};
	
	std::list<int> VisitedNodeIds{};
	std::map<int, int> ParentMap{};
	
	QueueNodeIds.push(StartNode->GetId());
	VisitedNodeIds.push_back(StartNode->GetId());
	
	int CurrentNodeId = Graphs::InvalidNodeId;
	
	while (!QueueNodeIds.empty())
	{
		CurrentNodeId = QueueNodeIds.front();
		QueueNodeIds.pop();
		
		if (CurrentNodeId == DestinationNode->GetId())
		{
			return ReconstructPath(ParentMap, StartNode, DestinationNode);
		}
		
		for (int NeighborId : NeighborIds)
		{
			const bool HasBeenVisited = ConversionHelpers::Contains(VisitedNodeIds, NeighborId);
			if (HasBeenVisited)
			{
				continue;
			}
			
			VisitedNodeIds.push_back(NeighborId);
			ParentMap[NeighborId] = CurrentNodeId;
			QueueNodeIds.push(NeighborId);
		}
		
	}
	
	return std::vector<Node*>();
}

std::vector<Node*> BFS::ReconstructPath(const std::map<int, int>& ParentMap, Node* const StartNode, Node* const GoalNode) const
{
	Node* CurrentNode = GoalNode;
	std::vector<Node*> Path;
	
	
	while (CurrentNode != StartNode)
	{
		Path.push_back(CurrentNode);
		
		const int CurrentNodeId = ParentMap.find(CurrentNode->GetId())->second;
		if (CurrentNodeId == Graphs::InvalidNodeId)
		{
			UE_LOG(LogTemp, Error, TEXT("ReconstructPath: CurrentNodeId is invalid"));
			return std::vector<Node*>();
		}
		
		CurrentNode = BfsGraph->GetNode(CurrentNodeId).get(); 
	}
	
	// Add startNode & reserve vector
	Path.push_back(StartNode);
	std::ranges::reverse(Path);
	
	return Path;
}
