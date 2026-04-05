#define UE_DISABLE_OPTIMIZATION
// TODO: remove this line after debugging
#include "AStar.h"

#include <list>
#include <queue>

using namespace GameAI;

AStar::AStar(Graph* const Graph, HeuristicFunctions::Heuristic Function)
	: AStarGraph(Graph)
	, HeuristicFunction(Function)
{
}

std::vector<Node*>AStar::FindPath(Node* const StartNode, Node* const GoalNode) const
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
	CurrentNodeRecord = StartRecord;
	ToBeCheckedNodeRecords.push_back(StartRecord);
	
	while (!ToBeCheckedNodeRecords.empty())
	{
		// std::ranges::min_element doesn't work due to NodeRecord not having
		// std::ranges::less implemented
		CurrentNodeRecord = *std::min_element(ToBeCheckedNodeRecords.begin(), ToBeCheckedNodeRecords.end());
		
		// if CurrentNode is Goal, then exit while loop
		if (CurrentNodeRecord.CurrentNode == GoalNode)
		{
			CheckedNodeRecords.push_back(CurrentNodeRecord);
			break;
		}
		
		// TODO: replace the previously used FindConnectionsTo to FindConnectionsWith, in previously used examples
		
		// for (Connection* CurrentConnection : AStarGraph->FindConnectionsWith(CurrentNodeRecord.CurrentNode->GetId()))
		for (Connection* CurrentConnection : AStarGraph->FindConnectionsFrom(CurrentNodeRecord.CurrentNode->GetId()))
		{
			// Aka neighbor node
			Node* const NextNode = AStarGraph->GetNode(CurrentConnection->GetToId()).get();
			
			const float CurrentGCost = CurrentNodeRecord.CostSoFar + GetHeuristicCost(CurrentNodeRecord.CurrentNode, NextNode);
			
			// Check if NextNode has already been checked
			const auto FoundCheckedRecordIt = std::ranges::find_if(CheckedNodeRecords, 
				[NextNode](const NodeRecord& CheckedNodeRecord)
				{
					if (CheckedNodeRecord.CurrentNode == NextNode)
					{
						return true;
					}
					
					return false;	
				});
			
			const bool bHasNodeAlreadyBeenChecked = (FoundCheckedRecordIt != CheckedNodeRecords.end()); 
			if (bHasNodeAlreadyBeenChecked)
			{
				
				UE_LOG(LogTemp, Warning, TEXT("NodeInToBeCheckedRecords"));
				// if an already existing connection to the same node is cheaper, skip this node
				// else, remove the existing connection, since it's bad
				if (CurrentGCost < FoundCheckedRecordIt->CostSoFar)
				{
					continue;
				}
				else
				{
					CheckedNodeRecords.erase(FoundCheckedRecordIt);	
				}
			}
			
			
			// Check if NextNode is in the ToBeCheck list
			const auto FoundToBeCheckedRecordIt = std::ranges::find_if(ToBeCheckedNodeRecords, 
				[NextNode](const NodeRecord& ToBeCheckedNodeRecord)
				{
					if (ToBeCheckedNodeRecord.CurrentNode == NextNode)
					{
						return true;
					}
					
					return false;	
				});
			
			const bool bIsNodeInToBeCheckedRecords = (FoundToBeCheckedRecordIt != ToBeCheckedNodeRecords.end()); 
			if (bIsNodeInToBeCheckedRecords)
			{
				UE_LOG(LogTemp, Warning, TEXT("NodeInToBeCheckedRecords"));
				if (CurrentGCost < FoundToBeCheckedRecordIt->CostSoFar)
				{
					continue;
				}
				else
				{
					ToBeCheckedNodeRecords.erase(FoundToBeCheckedRecordIt);	
				}
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
		
		ToBeCheckedNodeRecords.remove(CurrentNodeRecord);
		CheckedNodeRecords.push_back(CurrentNodeRecord);
	}
	
	if (CheckedNodeRecords.empty())
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckedNodeRecords is empty"));
		return std::vector<Node*>();
	}
	
			// todo: remove after find bug 
			FString ListText{};
			for (const NodeRecord& VisitedNodeId : CheckedNodeRecords)
			{
				if (!ListText.IsEmpty())
				{
					ListText += TEXT(", ");
				}
				
				ListText += FString::FromInt(VisitedNodeId.CurrentNode->GetId());
			}
			UE_LOG(LogTemp, Warning, TEXT("CheckedNodeRecords:"));
			UE_LOGFMT(LogTemp, Warning, "{VisitedNodeIds}", ListText);		
				
	
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
			UE_LOGFMT(LogTemp, Warning, "CurrentNode Id is {Id}", CurrentNodeRecord.CurrentNode->GetId());
			UE_LOGFMT(LogTemp, Warning, "PreviousNodeId is {Id}", PreviousNodeId);
		
		
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
			UE_LOG(LogTemp, Warning, TEXT("PreviousNodeRecordIt is nullptr, Ahhhh"));
			// return std::vector<Node*>{};
			break;
		}
		
		CurrentNodeRecord = *PreviousNodeRecordIt;
	}
	
	Path.push_back(StartNode);
	std::ranges::reverse(Path);
	
	return Path;
}

float AStar::GetHeuristicCost(Node* const StartNode, Node* const EndNode) const
{
	const FVector2D ToDestination = AStarGraph->GetNode(EndNode->GetId())->GetPosition() - AStarGraph->GetNode(StartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(ToDestination.X), abs(ToDestination.Y));
}