#include "AStar.h"

#include <list>
#include <queue>

using namespace GameAI;


AStar::AStar(Graph* const Graph, const HeuristicFunctions::Heuristic Function)
	: AStarGraph(Graph)
	, HeuristicFunction(Function)
{
}

std::vector<Node*>AStar::FindPath(Node* const StartNode, const Node* const GoalNode) const
{
	if (StartNode->GetId() == Graphs::InvalidNodeId)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode Id is invalid"));
		return std::vector<Node*>{};
	}	
		
	if (GoalNode->GetId() == Graphs::InvalidNodeId)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndNode Id is invalid"));
		return std::vector<Node*>{};
	}	
		
	if (StartNode == GoalNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode is same as EndNode"));
		return std::vector<Node*>{};
	}
	
	if (StartNode->GetId() == GoalNode->GetId())
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode Id is same as EndNode Id"));
		return std::vector<Node*>{};
	}
	
	
	std::list<NodeRecord> ToBeCheckedNodeRecords{};
	std::list<NodeRecord> CheckedNodeRecords{};
	NodeRecord CurrentNodeRecord{};
	
	const NodeRecord StartRecord
	{
		.CurrentNode = StartNode,
		.CurrentConnection = nullptr,
		.CostSoFar = 0.0f,
		.EstimatedTotalCost = 0.0f + GetHeuristicCost(StartNode, GoalNode),
	};
	ToBeCheckedNodeRecords.push_back(StartRecord);
	
	while (!ToBeCheckedNodeRecords.empty())
	{
		// std::ranges::min_element doesn't work due to NodeRecord not having
		// std::ranges::less implemented
		auto CurrentNodeRecordIt = std::min_element(ToBeCheckedNodeRecords.begin(), ToBeCheckedNodeRecords.end());
		CurrentNodeRecord = *CurrentNodeRecordIt;
		
		// if CurrentNode is Goal, then exit while loop
		if (CurrentNodeRecord.CurrentNode == GoalNode)
		{
			break;
		}
		
		for (Connection* CurrentConnection : AStarGraph->FindConnectionsFrom(CurrentNodeRecord.CurrentNode->GetId()))
		{
			// Aka neighbor node
			Node* const NextNode = AStarGraph->GetNode(CurrentConnection->GetToId()).get();
			
			const float CurrentGCost = CurrentNodeRecord.CostSoFar + CurrentConnection->GetWeight();
			
			// Check if NextNode has already been checked
			const auto FoundCheckedRecordIt = std::ranges::find_if(CheckedNodeRecords, 
				[&NextNode](const NodeRecord& CheckedNodeRecord)
				{
					return CheckedNodeRecord.CurrentNode == NextNode;
				});
			
			const bool bHasNodeAlreadyBeenChecked = FoundCheckedRecordIt != CheckedNodeRecords.end(); 
			if (bHasNodeAlreadyBeenChecked)
			{
				UE_LOG(LogTemp, Warning, TEXT("NodeInToBeCheckedRecords"));
					
				// If the node is already checked and the checked version's cost is same or cheaper, skip this node
				if (FoundCheckedRecordIt->CostSoFar <= CurrentGCost)
				{
					continue;
				}
				
				CheckedNodeRecords.remove(*FoundCheckedRecordIt);	
			}
				
			// Check if NextNode is in the ToBeCheck list
			const auto FoundToBeCheckedRecordIt = std::ranges::find_if(ToBeCheckedNodeRecords, 
				[&NextNode](const NodeRecord& ToBeCheckedNodeRecord)
				{
					return ToBeCheckedNodeRecord.CurrentNode == NextNode;
				});
			
			const bool bIsNodeInToBeCheckedRecords = FoundToBeCheckedRecordIt != ToBeCheckedNodeRecords.end(); 
			if (bIsNodeInToBeCheckedRecords)
			{
				if (FoundToBeCheckedRecordIt->CostSoFar <= CurrentGCost)
				{
					continue;
				}
				
				ToBeCheckedNodeRecords.remove(*FoundToBeCheckedRecordIt);	
			}
			
			
			NodeRecord ConnectionRecord
			{
				.CurrentNode = NextNode,
				.CurrentConnection = CurrentConnection,
				.CostSoFar = CurrentGCost,
				.EstimatedTotalCost = CurrentGCost + GetHeuristicCost(NextNode, GoalNode),
			};
			
			ToBeCheckedNodeRecords.push_back(ConnectionRecord);
		}
		
		if (CurrentNodeRecordIt == ToBeCheckedNodeRecords.end())
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentNodeRecordIt is invalid"));
			return std::vector<Node*>{};	
		}
		
		ToBeCheckedNodeRecords.remove(CurrentNodeRecord);
		CheckedNodeRecords.push_back(CurrentNodeRecord);
	}
	
	
	if (CheckedNodeRecords.empty())
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckedNodeRecords is empty"));
		return std::vector<Node*>();
	}
	
	
	// Backtracking
	std::vector<Node*> Path{};
	
	while (CurrentNodeRecord.CurrentNode != StartNode)
	{
		if (CurrentNodeRecord.CurrentNode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentNode is nullptr"));
			return std::vector<Node*>{};
		}
		
		Path.push_back(CurrentNodeRecord.CurrentNode);
		
		const int PreviousNodeId = CurrentNodeRecord.CurrentConnection->GetFromId();
		const auto PreviousNodeRecordIt = std::ranges::find_if(CheckedNodeRecords,
			[PreviousNodeId](const NodeRecord& CheckedNodeRecord)
			{
				if (CheckedNodeRecord.CurrentNode->GetId() == PreviousNodeId)
				{
					return true;
				}
				
				return false;
			});
		
		const bool bHasFoundPreviousRecord = (PreviousNodeRecordIt != CheckedNodeRecords.end());
		if (!bHasFoundPreviousRecord)
		{
			UE_LOG(LogTemp, Warning, TEXT("PreviousNodeRecordIt is nullptr"));
			return std::vector<Node*>{};
		}
		
		CurrentNodeRecord = *PreviousNodeRecordIt;
	}
	
	Path.push_back(StartNode);
	std::ranges::reverse(Path);
	
	return Path;
}

float AStar::GetHeuristicCost(const Node* const StartNode, const Node* const EndNode) const
{
	const FVector2D ToDestination = AStarGraph->GetNode(EndNode->GetId())->GetPosition() - AStarGraph->GetNode(StartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(ToDestination.X), abs(ToDestination.Y));
}

	
