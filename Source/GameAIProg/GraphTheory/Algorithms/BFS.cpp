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

// Breath First Search Algorithm searches for a path from the startNode to the destinationNode
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
	
	std::queue<int> QueueNodeIds{};		// OpenList
	
	std::list<int> VisitedNodeIds{};	// ClosedList
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
		
		for (int NeighborId : GetNeighborsIds(CurrentNodeId))
		{
			const bool HasBeenVisited = ConversionHelpers::Contains(VisitedNodeIds, NeighborId);
			if (HasBeenVisited)
			{
				FString ListText{};
				for (const int VisitedNodeId : VisitedNodeIds)
				{
					if (!ListText.IsEmpty())
					{
						ListText += TEXT(", ");
					}
					
					ListText += FString::FromInt(VisitedNodeId);
				}
				
				UE_LOGFMT(LogTemp, Verbose, "HasBeenVisited true, NeighborId({NeighborId}), ", NeighborId);		
				UE_LOGFMT(LogTemp, Verbose, "VisitedNodeId List: {VisitedNodeIds}", ListText);		
				
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

std::vector<int> BFS::GetNeighborsIds(int NodeId) const
{
	if (NodeId == Graphs::InvalidNodeId)
	{
		UE_LOG(LogTemp, Error, TEXT("BFS::GetNeighborsIds: CurrentNode.Id is invalid"));
		return std::vector<int>();
	}
	
	const auto& ToConnections = BfsGraph->FindConnectionsFrom(NodeId);
	if (ToConnections.empty())
	{
		UE_LOG(LogTemp, Warning, TEXT("BFS::GetNeighborsIds: No connections found"));
		return std::vector<int>();
	}
	
	std::vector<int> NeighborIds(ToConnections.size(), Graphs::InvalidNodeId);
	for (int i = 0; i < ToConnections.size(); ++i)
	{
		if (!ToConnections[i])
		{
			continue;
		}
		
		NeighborIds[i] = ToConnections[i]->GetToId();
	}
	
	return NeighborIds;	
}
