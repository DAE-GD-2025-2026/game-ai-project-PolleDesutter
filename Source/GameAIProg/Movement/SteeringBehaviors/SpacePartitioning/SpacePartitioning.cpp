#include "SpacePartitioning.h"

#include "GeometryCollection/GeometryCollectionConvexUtility.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{ pWorld }
	, SpaceWidth{ Width }
	, SpaceHeight{ Height }
	, NrOfRows{ Rows }
	, NrOfCols{ Cols }
	, NrOfNeighbors{}
{
	Neighbors.SetNum(MaxEntities);
	
	// Calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	// TODO: create the cells
	for (int ColumnIdx = 0; ColumnIdx < NrOfCols; ++ColumnIdx)
	{
		for (int RowIdx  = 0; RowIdx < NrOfRows; ++RowIdx)
		{
			Cell CurrentCell{ ColumnIdx * CellWidth, RowIdx * CellHeight, CellWidth, CellHeight };
			Cells.emplace_back(CurrentCell);
		}
	}
	
	
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	// TODO: Add the agent to the correct cell
	const int CellIndex = PositionToIndex(Agent.GetPosition());
	
	if (CellIndex < 0 || CellIndex >= Cells.size())
	{
		UE_LOG(LogTemp, Warning, TEXT("AddAgent: CellIndex is invalid"));
		return;		
	}
	
	Cells[CellIndex].Agents.push_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
	const int OldCellIndex = PositionToIndex(OldPos);
	const int NewCellIndex = PositionToIndex(Agent.GetPosition());
	
	if (OldCellIndex ==	NewCellIndex)
	{
		return;	
	}
	
	Cells[OldCellIndex].Agents.remove(&Agent);
	Cells[NewCellIndex].Agents.push_back(&Agent);
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	// TODO Register the neighbors for the provided agent
	// TODO Only check the cells that are within the radius of the neighborhood
	NrOfNeighbors = 0;	
	
	const FVector2D AgentPosition = Agent.GetPosition();
	const int CellIndex = PositionToIndex(AgentPosition);
	
	// if cell invalid
	if (CellIndex == -1)
	{
		return;
	}
	
	const Cell& CurrentCell = Cells[CellIndex]; 
	const FRect CurrentCellBoundingBox = CurrentCell.BoundingBox; 
	
	for (const auto& OtherCell : Cells)
	{
		if (&CurrentCell == &OtherCell)
		{
			continue;
		}
		
		if (!DoRectsOverlap(CurrentCellBoundingBox, OtherCell.BoundingBox))
		{
			continue;
		}
		
		
		for (const auto& Neighbor : OtherCell.Agents)
		{
			if (!Neighbor)
			{
				continue;	
			}
			
			if (FVector2D::Distance(AgentPosition, Neighbor->GetPosition())  > QueryRadius)
			{
				continue;
			}
			
			if (NrOfNeighbors >= Neighbors.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("Neighbor Array is too Small"));
				return;
			}
			
			Neighbors[NrOfNeighbors] = Neighbor;
			++NrOfNeighbors;		
		
		}
	}
	
	
}

void CellSpace::EmptyCells()
{
	for (Cell& Cell : Cells)
	{
		Cell.Agents.clear();
	}
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it
}

int CellSpace::PositionToIndex(FVector2D const& Pos) const
{
	// TODO Calculate the index of the cell based on the position
	const FVector2D Position = Pos - CellOrigin;
	const int ColumnIdx = static_cast<int>(Position.X / CellWidth);	
	const int RowIdx	= static_cast<int>(Position.Y / CellHeight);	
	
	if (ColumnIdx < 0 || ColumnIdx >= NrOfCols)
	{
		UE_LOG(LogTemp, Warning, TEXT("PositionToIndex: ColumnIdx out of range"));
		return -1;
	}
		
	if (RowIdx < 0 || RowIdx >= NrOfRows)
	{
		UE_LOG(LogTemp, Warning, TEXT("PositionToIndex: RowsIdx out of range"));
		return -1;
	}
	
	return ColumnIdx + RowIdx * NrOfCols;
}

bool CellSpace::DoRectsOverlap(FRect const& RectA, FRect const& RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}