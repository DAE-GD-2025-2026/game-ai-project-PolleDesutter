#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Kismet/KismetMathLibrary.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
	{
	public:
		//=== SSFA Functions ===
		//--- References ---
		//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
		//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
		static std::vector<NavLine> FindPortals(std::vector<Node*> const& Path, TriPolygon const& NavPoly)
		{
			// Container
			std::vector<NavLine> Portals = {};

			if (Path.empty())
			{
				UE_LOGFMT(LogTemp, Warning, "Empty Path");
				return Portals;
			}
			
			const FVector2D ApexPoint = Path[0]->GetPosition();
			
			Portals.push_back(NavLine(ApexPoint, ApexPoint));
			
			// For each node received, get it's corresponding line
			for (const auto Node : Path)
			{
				const auto NavNode = reinterpret_cast<NavGraphNode*>(Node);
				const int EdgeIndex = NavNode->GetEdgeIdx();

				if (EdgeIndex < 0 || EdgeIndex >= NavPoly.GetEdges().size())
				{
					continue;
				}
			
				const TriPolygon::Edge Edge = NavPoly.GetEdges()[EdgeIndex];

				const FVector2D EdgeP1 = FVector2D(Edge.GetP1(NavPoly));
				const FVector2D EdgeP2 = FVector2D(Edge.GetP2(NavPoly));
				
				const FVector2D EdgeP1Center = EdgeP1 - ApexPoint;
				const FVector2D EdgeP2Center = EdgeP2 - ApexPoint;
				
				//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
				const float CrossProduct = FVector2D::CrossProduct(EdgeP1Center.GetSafeNormal(), EdgeP2Center.GetSafeNormal());

				
				FVector2D P1{};
				FVector2D P2{};
				
				// P1 is on right (good)
				if (CrossProduct <= 0)
				{
					P1 = EdgeP1;
					P2 = EdgeP2;
				}
				
				// P1 is on left (bad, switch)
				if (CrossProduct > 0)
				{
					P1 = EdgeP2;
					P2 = EdgeP1;
				}
					
					
				// Store portal
				Portals.emplace_back(P1, P2);
			}
			
			// Add degenerate portal to force end evaluation
			const FVector2D EndPosition = Path[Path.size() - 1]->GetPosition();
			Portals.emplace_back(NavLine(EndPosition, EndPosition));
			
			return Portals;
		}

		static std::vector<FVector2D> OptimizePortals(std::vector<NavLine> const& Portals, TriPolygon const&)
		{
			std::vector<FVector2D> Path{};
			Path.push_back(Portals.front().P1);
			
			
			// P1 == right point of portal, P2 == left point of portal
			int RightLegIndex = 1;
			int LeftLegIndex  = 1;
			
			FVector2D ApexPos = Portals[0].P1;
			FVector2D RightLegPos = Portals[RightLegIndex].P1 - ApexPos;
			FVector2D LeftLegPos = Portals[LeftLegIndex].P2 - ApexPos;
			
			for(int PortalIndex = 2; PortalIndex < Portals.size(); ++PortalIndex)
			{
				const NavLine CurrentPortal = Portals[PortalIndex];				
				
				//--- RIGHT CHECK ---
				//1. See if moving funnel inwards - RIGHT
				const FVector2D NewRightLegPos = CurrentPortal.P1 - ApexPos;
				if (FVector2D::CrossProduct(RightLegPos, NewRightLegPos) <= 0.f)
				{
					//2. See if new line degenerates a line segment - RIGHT
					if (FVector2D::CrossProduct(LeftLegPos, NewRightLegPos) < 0.f)
					{
						// LeftLeg becomes new apex point
						// Right Leg Crossing
						ApexPos += LeftLegPos;
						
						++LeftLegIndex;
						PortalIndex = LeftLegIndex;
						RightLegIndex = LeftLegIndex;
						
						Path.push_back(ApexPos);
						
						// Calculate new legs (if not the end)
						if (PortalIndex < Portals.size())
						{
							RightLegPos = Portals[RightLegIndex].P1 - ApexPos;
							LeftLegPos = Portals[LeftLegIndex].P2 - ApexPos;
							continue;
						}
						
					}
					else
					{
						// not crossing
						RightLegPos = NewRightLegPos;
						RightLegIndex = PortalIndex;
					}
				}


				//--- LEFT CHECK ---
				//1. See if moving funnel inwards - LEFT
				const FVector2D NewLeftLegPos = CurrentPortal.P2 - ApexPos;
				if (FVector2D::CrossProduct(LeftLegPos, NewLeftLegPos) >= 0.f)
				{
					//2. See if new line degenerates a line segment - LEFT
					if (FVector2D::CrossProduct(RightLegPos, NewLeftLegPos) > 0.f)
					{
						// RightLeg becomes new apex point
						// Left Leg Crossing
						ApexPos += RightLegPos;
						
						++RightLegIndex;
						PortalIndex = RightLegIndex;
						LeftLegIndex = PortalIndex;
						
						Path.push_back(ApexPos);
						
						// Calculate new legs (if not the end)
						if (PortalIndex < Portals.size())
						{
							RightLegPos = Portals[RightLegIndex].P1 - ApexPos;
							LeftLegPos = Portals[LeftLegIndex].P2 - ApexPos;
							continue;
						}
					}
					else
					{
						// not crossing
						LeftLegPos = NewLeftLegPos;
						LeftLegIndex = PortalIndex;
					}
				}
			
			}
			
			// Add last path point
			Path.push_back(Portals.back().P1);

			return Path;
			//*/
		}
	private
	:
	SSFA()
		{
		};
		~SSFA()
		{
		};
	};
}
