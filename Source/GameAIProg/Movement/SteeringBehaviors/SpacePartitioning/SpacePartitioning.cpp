#include "SpacePartitioning.h"

#include "GeometryCollection/GeometryCollectionConvexUtility.h"

// NOTE: Y is horizontal and X is Vertical (for some reason)

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = {Left, Bottom};
	BoundingBox.Max = {BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height};
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float Left = BoundingBox.Min.X;
	const float Bottom = BoundingBox.Min.Y;
	const float Width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float Height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> RectPoints =
	{
		{Left, Bottom},
		{Left, Bottom + Height},
		{Left + Width, Bottom + Height},
		{Left + Width, Bottom},
	};

	return RectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	  , SpaceWidth{Width}
	  , SpaceHeight{Height}
	  , NrOfRows{Rows}
	  , NrOfCols{Cols}
	  , NrOfNeighbors{}
{
	Neighbors.SetNum(MaxEntities);

	// Calculate bounds of a cell
	CellWidth = SpaceWidth / Cols;
	CellHeight = SpaceHeight / Rows;

	for (int RowIdx = 0; RowIdx < NrOfRows; ++RowIdx)
	{
		for (int ColumnIdx = 0; ColumnIdx < NrOfCols; ++ColumnIdx)
		{
			Cell CurrentCell
			{
				-SpaceHeight / 2.f + RowIdx * CellHeight,
				-SpaceWidth / 2.f + ColumnIdx * CellWidth,
				CellHeight,
				CellWidth,
			};
			Cells.emplace_back(CurrentCell);
		}
	}

	// for (const auto& Cell : Cells)
	// {
	// 	DrawDebugPoint(pWorld, FVector(UnrealHelpers::GetCenter(Cell.BoundingBox), 0), 
	// 		ConstantHelpers::DebugDefaultPointSize, FColor::Green, true);
	// }
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	const int CellIndex = PositionToIndex(Agent.GetPosition());

	if (CellIndex < 0 || CellIndex >= Cells.size())
	{
		UE_LOG(LogTemp, Error, TEXT("AddAgent: CellIndex is invalid"));
		UnrealHelpers::QuitGameOrPie(Agent.GetWorld());
		return;
	}

	Cells[CellIndex].Agents.push_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	const int OldCellIndex = PositionToIndex(OldPos);
	int NewCellIndex = PositionToIndex(Agent.GetPosition());

	if (OldCellIndex == NewCellIndex)
	{
		return;
	}

	// Due to the late 'teleporting', the OldCellIndex can be -1.
	// e.g. Agent is exits the TrimWorld, before the teleporting, 
	// the NewIndex will be -1, and the Agent's entry at OldCellIndex 
	// will need to be removed. Same for when OldCellIndex is -1, 
	// and then the teleport happens, meaning NewCellIndex will be valid.

	if (OldCellIndex == -1)
	{
		UE_LOGFMT(LogTemp, Warning, "OldCellIndex is invalid (-1)");
		return;
	}
	if (NewCellIndex == -1)
	{
		// waiting until teleport happens, this didn't seem to fix the issue
		UE_LOGFMT(LogTemp, Log, "NewCellIndex is invalid (-1)");

		const FVector2D Position = Agent.GetPosition() + FVector2D(SpaceWidth / 2.f, SpaceHeight / 2.f);
		const int ColumnIdx = static_cast<int>(Position.Y / CellWidth);
		const int RowIdx = static_cast<int>(Position.X / CellHeight);

		// this clamps the index to the closest valid
		const int ClampedCellIndex = FMath::Clamp(ColumnIdx, 0, NrOfCols - 1) +
			FMath::Clamp(RowIdx, 0, NrOfRows - 1) * NrOfCols;

		int FixedNewIndex = -1;
		if (ColumnIdx < 0)
		{
			// Add agent to right column cell 
			FixedNewIndex = ClampedCellIndex + (NrOfCols - 1);
		}
		else if (ColumnIdx >= NrOfCols)
		{
			// Add agent to left column cell 
			FixedNewIndex = ClampedCellIndex - (NrOfCols - 1);
		}
		else if (RowIdx < 0)
		{
			// Add agent to top row cell
			FixedNewIndex = ClampedCellIndex + NrOfCols * (NrOfRows - 1);
		}
		else if (RowIdx >= NrOfRows)
		{
			// Add agent to bottom row cell
			FixedNewIndex = ClampedCellIndex - NrOfCols * (NrOfRows - 1);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateAgentCell: RowIdx or CellIdx both not invalid"));
		}
		
		if (FixedNewIndex < 0 || FixedNewIndex >= Cells.size())
		{
			UE_LOG(LogTemp, Error, TEXT("UpdateAgentCell: FixedIndex is invalid"));
			return;
		}
	
		// remove from old cell 
		Cells[OldCellIndex].Agents.remove(&Agent);
		Cells[FixedNewIndex].Agents.push_back(&Agent);

		return;
	}

	if (Cells.size() <= OldCellIndex)
	{
		UE_LOGFMT(LogTemp, Error, "OldCellIndex ({CellIndex}) bigger than Cell size", OldCellIndex);
		return;
	}
	if (Cells.size() <= NewCellIndex)
	{
		UE_LOGFMT(LogTemp, Error, "NewCellIndex ({CellIndex}) bigger than Cell size", NewCellIndex);
		return;
	}

	if (OldCellIndex < 0)
	{
		UE_LOGFMT(LogTemp, Error, "OldCellIndex ({CellIndex}) less than 0", OldCellIndex);
		return;
	}
	if (NewCellIndex < 0)
	{
		UE_LOGFMT(LogTemp, Error, "NewCellIndex ({CellIndex}) less than 0", NewCellIndex);
		return;
	}


	Cells[OldCellIndex].Agents.remove(&Agent);
	Cells[NewCellIndex].Agents.push_back(&Agent);
}

void CellSpace::RegisterNeighbors(const ASteeringAgent& Agent, float QueryRadius)
{
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

			if (FVector2D::Distance(AgentPosition, Neighbor->GetPosition()) > QueryRadius)
			{
				continue;
			}

			if (NrOfNeighbors >= Neighbors.Num())
			{
				UE_LOG(LogTemp, Error, TEXT("Neighbor Array is too Small"));
				// UnrealHelpers::QuitGameOrPie(pWorld->GetWorld());
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
	const FVector Extent(CellHeight / 2.f, CellWidth / 2.f, 0.0f);

	for (const auto& Cell : Cells)
	{
		const FVector Center = FVector(UnrealHelpers::GetCenter(Cell.BoundingBox), 0);
		DrawDebugBox(pWorld, Center, Extent, FColor::Magenta);

		const int NrOfAgents = Cell.Agents.size();
		const FVector CenterOffsetText = FVector(Extent.Y / 2.f, -Extent.X / 2.f, 0.f);
		DrawDebugString(pWorld, Center + CenterOffsetText, FString::FromInt(NrOfAgents), 
			nullptr, FColor::Red, 0.01f, false, 1.2f);
	}
}

bool CellSpace::IsPositionInPartitionSpace(const FVector2D& Pos) const
{
	const int Index = PositionToIndex(Pos);
	const bool IsInPartitionSpace = (Index != -1);

	return IsInPartitionSpace;
}

int CellSpace::PositionToIndex(FVector2D const& Pos) const
{
	const FVector2D Position = Pos + FVector2D(SpaceWidth / 2.f, SpaceHeight / 2.f);
	const int ColumnIdx = static_cast<int>(Position.Y / CellWidth);
	const int RowIdx = static_cast<int>(Position.X / CellHeight);

	if (ColumnIdx < 0 || ColumnIdx >= NrOfCols)
	{
		UE_LOGFMT(LogTemp, Warning, "PositionToIndex: ColumnIdx ({ColumnIndex}) out of range", ColumnIdx);
		return -1;
	}

	if (RowIdx < 0 || RowIdx >= NrOfRows)
	{
		UE_LOGFMT(LogTemp, Warning, "PositionToIndex: RowIdx ({RowIndex}) out of range", RowIdx);
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
