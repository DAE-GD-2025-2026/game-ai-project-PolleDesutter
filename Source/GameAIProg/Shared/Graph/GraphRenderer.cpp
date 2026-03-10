#include "GraphRenderer.h"

namespace GameAI
{
	GraphRenderer::GraphRenderer(UWorld* pWorld)
	: World{ pWorld }
	{
	}

	void GraphRenderer::SetRenderOptions(const GraphRenderOptions& NewOptions)
	{
		Options = NewOptions;
	}

	void GraphRenderer::RenderGraph(const Graph&  Graph) const
	{
		if (Options.bDrawNodes)
		{
			for (auto & Node : Graph.GetNodes())
			{
				if (Node->GetId() != Graphs::InvalidNodeId)
				{
					// We skip invalid nodes
					DrawNode(*Node);
				}
			}
		}

		if (Options.bDrawNodes)
		{
			for (const auto& Connection : Graph.GetConnections())
			{
				DrawConnection(Graph, *Connection);
			}
		}

	}
}

