#pragma once
#include <vector>

namespace GameAI
{
	class NavGraph;

	struct NavLine
	{
		FVector2D P1;
		FVector2D P2;	
	};

	class NavMeshPathfinding
	{
	public:
		static std::vector<FVector2D> FindPath(const FVector2D& StartPos, const FVector2D& EndPos, const NavGraph* const NavigationGraph,
		                                       std::vector<FVector2D>& DebugNodePositions, std::vector<NavLine>& DebugPortals);
		static std::vector<FVector2D> FindPath(const FVector2D& StartPos, const FVector2D& EndPos, const NavGraph* const NavigationGraph);
	};
}
