#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(const int EdgeIdx) const
{
	if (EdgeIdx < 0)
	{
		return Graphs::InvalidNodeId;
	}
	
	for (const auto& Node : Nodes)
	{
		const auto NavNode = reinterpret_cast<NavGraphNode*>(Node.get());
		if (NavNode->GetEdgeIdx() == EdgeIdx)
		{
			return Node->GetId();
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	// Going over all the edges of the navigation mesh and create nodes
	int EdgeLoopIdx{};
	for (const TriPolygon::Edge& CurrentEdge : pNavPoly->GetEdges())
	{
		// Creating node here
		const FVector EdgeP1 = CurrentEdge.GetP1(*pNavPoly);
		const FVector EdgeP2 = CurrentEdge.GetP2(*pNavPoly);
		const FVector MiddleEdgePos = (EdgeP1 + EdgeP2) / 2.f;
		
		auto UniqueNavNode = std::make_unique<NavGraphNode>(FVector2D(MiddleEdgePos), EdgeLoopIdx);
		AddNode(std::move(UniqueNavNode));
		
		++EdgeLoopIdx;	
	}

	// Creating connections now that every node is created	
	for (const TriPolygon::Triangle& Triangle : pNavPoly->GetTriangles())
	{
		std::vector<int> NodeIds{};
		
		for (const TriPolygon::Edge& Edge : Triangle.GetEdges())
		{
			std::optional<int> EdgeIdxOptional = pNavPoly->FindEdgeIndex(Edge);
			if (EdgeIdxOptional.has_value())
			{
				const int EdgeIdx = EdgeIdxOptional.value();
				if (EdgeIdx == Graphs::InvalidNodeId)
				{
					continue;
				}
				
				const int NodeIdx = GetNodeIdFromEdgeIndex(EdgeIdx);
				if (NodeIdx == Graphs::InvalidNodeId)
				{
					continue;
				}
				
				NodeIds.push_back(NodeIdx);
			}
		}
		
		// 2 valid nodes -> 1 connection
		// 3 valid nodes -> 3 connections
		if (NodeIds.size() == 2)
		{
			auto NodeConnection = std::make_unique<Connection>(NodeIds[0], NodeIds[1]);
			AddConnection(std::move(NodeConnection));
		}
		else if (NodeIds.size() == 3)
		{
			auto NodeConnection = std::make_unique<Connection>(NodeIds[0], NodeIds[1]);
			AddConnection(std::move(NodeConnection));
			
			NodeConnection = std::make_unique<Connection>(NodeIds[1], NodeIds[2]);
			AddConnection(std::move(NodeConnection));
			
			NodeConnection = std::make_unique<Connection>(NodeIds[2], NodeIds[0]);
			AddConnection(std::move(NodeConnection));
		}
	}
		
	// Setting the connections cost to the actual distance
	SetConnectionCostsToDistances();
	
}
