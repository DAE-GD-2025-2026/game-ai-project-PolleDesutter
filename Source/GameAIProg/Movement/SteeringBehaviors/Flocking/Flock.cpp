#include "Flock.h"

#include "Level_Flocking.h"
#include "Shared/WorldTrimVolume.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(UWorld* pWorld, TSubclassOf<ASteeringAgent> AgentClass, int FlockSize,
             float WorldSize, ASteeringAgent* const pAgentToEvade, bool bTrimWorld)
	: pWorld{pWorld}
	  , SteeringAgentClass{AgentClass}
	  , FlockSize{FlockSize}
	  , pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);
	
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	pPartitionedSpace = std::make_unique<CellSpace>(pWorld, WorldSize, WorldSize, NrOfCellsX, NrOfCellsX, FlockSize);
#else
	Neighbors.SetNum(FlockSize - 1);
#endif	

	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior	= std::make_unique<Cohesion>(this);
	pVelMatchBehavior	= std::make_unique<VelocityMatch>(this);

	pSeekBehavior			= std::make_unique<Seek>();
	pWanderBehavior			= std::make_unique<Wander>();
	pEvadeNearbyBehavior	= std::make_unique<EvadeNearby>();

	if (!pWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("BaseLevelScriptActor is not valid"))
		UnrealHelpers::QuitGameOrPie(pWorld->GetWorld());
		return;
	}

	if (!pAgentToEvade)
	{
		UE_LOG(LogTemp, Error, TEXT("AgentToEvade is not valid"))
		UnrealHelpers::QuitGameOrPie(pWorld->GetWorld());
		return;
	}

	pAgentToEvade->SetSteeringBehavior(pWanderBehavior.get());
	pAgentToEvade->SetBodyMaterial(pAgentToEvade->GetRedBodyMaterial());
	pAgentToEvade->SetDebugRenderingEnabled(DebugRenderSteering);	

	const auto Level = pWorld->GetLevel(0);
	if (Level)
	{
		const auto LevelScriptActor = Level->GetLevelScriptActor();
		if (LevelScriptActor)
		{
			BaseLevelScriptActor = Cast<ALevel_Base>(LevelScriptActor);
		}
	}

	if (BaseLevelScriptActor)
	{
		pTrimWorld = BaseLevelScriptActor->GetTrimWorld();
	}

	if (!BaseLevelScriptActor)
	{
		UE_LOG(LogTemp, Error, TEXT("BaseLevelScriptActor is not valid"))
		UnrealHelpers::QuitGameOrPie(pWorld->GetWorld());
		return;
	}
	if (!pTrimWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("TrimWorld is not valid"))
		UnrealHelpers::QuitGameOrPie(pWorld->GetWorld());
		return;
	}


	const std::vector<BlendedSteering::WeightedBehavior> FlockWeightedBehaviors
	{
		{ pCohesionBehavior.get(),	0.5f	},
		{ pSeparationBehavior.get(), 0.5f	},
		{ pVelMatchBehavior.get(),	0.5f	},
		{ pSeekBehavior.get(),		0.5f	},
		{ pWanderBehavior.get(),		0.5f	},
	};

	pBlendedSteering = std::make_unique<BlendedSteering>(FlockWeightedBehaviors);

	const std::vector<ISteeringBehavior*> PrioritySteeringBehaviors
	{
		pEvadeNearbyBehavior.get(),
		pBlendedSteering.get()
	};

	pPrioritySteering = std::make_unique<PrioritySteering>(PrioritySteeringBehaviors);

	for (int i = 0; i < FlockSize; ++i)
	{
		// // We don't care if bTrimWorld is set or not,
		// // we still spawn only in world size
		
		const FVector RandomSpawnPosition =
		{
			FMath::RandRange(-WorldSize, WorldSize),
			FMath::RandRange(-WorldSize, WorldSize),
			90
		};

		ASteeringAgent* Agent = pWorld->SpawnActor<ASteeringAgent>(SteeringAgentClass, RandomSpawnPosition,
		                                                           FRotator::ZeroRotator);

		if (!Agent)
		{
			UE_LOGFMT(LogTemp, Warning, "Agent {Index} failed to spawn at location: \t {SpawnPosition}", i,
			          RandomSpawnPosition.ToString());
			continue;
		}

		Agent->SetBodyMaterial(Agent->GetNormalBodyMaterial());
		Agent->SetSteeringBehavior(pPrioritySteering.get());
		Agent->SetDebugRenderingEnabled(DebugRenderSteering);	

		UE_LOGFMT(LogTemp, Verbose, "Agent {Index} Location:\t{Location}", i, RandomSpawnPosition.ToString());

		Agents[i] = Agent;
	}
}

Flock::~Flock()
{
	for (const auto Agent : Agents)
	{
		if (Agent)
			Agent->Destroy();
	}
}

void Flock::Tick(float DeltaTime)
{
	if (pEvadeNearbyBehavior)
	{
		FTargetData TargetData;
		TargetData.Position = pAgentToEvade->GetPosition();
		TargetData.Orientation = pAgentToEvade->GetRotation();
		TargetData.LinearVelocity = pAgentToEvade->GetLinearVelocity();
		TargetData.AngularVelocity = pAgentToEvade->GetAngularVelocity();

		pEvadeNearbyBehavior->SetTarget(TargetData);
	}

	for (auto pAgent : Agents)
	{
		if (!pAgent)
		{
			continue;
		}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
		RegisterNeighbors(pAgent);
#endif
		
		pAgent->Tick(DeltaTime);

	}
	
}

void Flock::RenderDebug()
{
	for (auto pAgent : Agents)
	{
		if (!pAgent)
		{
			return;
		}
		
		if (DebugRenderNeighborhood)
		{
			RenderNeighborhood();
		}
		
		if (DebugRenderEvadeRadius)
		{
			DrawDebugCircle(pWorld, FVector(pAgentToEvade->GetPosition(), 0), pEvadeNearbyBehavior->EvadeRadius,   ConstantHelpers::DebugDefaultCircleSegments, 
				FColor::Cyan,   false, 0.f, 0, 5.f, FVector(1,0,0), FVector(0,1,0), false);
		}

		// TODO: Add Debug when spacial partitioning is done
		if (DebugRenderPartitions)
		{
		}
	}
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive,
		             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scroll wheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

		if (BaseLevelScriptActor)
		{
			ImGui::Checkbox("Visualize Mouse Target", &BaseLevelScriptActor->VisualizeMouseTarget);
		}
		
		if (ImGui::Checkbox("Debug Neighborhood", &DebugRenderNeighborhood))
		{
			if (!DebugRenderNeighborhood)
			{
				for (const auto Agent : Agents)
				{
					if (!Agent)
					{
						continue;
					}
					
					Agent->SetBodyMaterial(Agent->GetNormalBodyMaterial());
				}
			}
			
		}
		
		if (ImGui::Checkbox("Debug Render Steering", &DebugRenderSteering))
		{
			pAgentToEvade->SetDebugRenderingEnabled(DebugRenderSteering);
			
			for (const auto Agent : Agents)
			{
				if (!Agent)
				{
					continue;
				}
				
				Agent->SetDebugRenderingEnabled(DebugRenderSteering);
			}
			
		}
		ImGui::Checkbox("Debug Show Evade Radius", &DebugRenderEvadeRadius);
		ImGui::Checkbox("Debug Show Average Flock Position", &DebugRenderAveragePosition);
		
		if (ImGui::Checkbox("Debug Behavior", &CanDebugBehavior))
		{
			pAgentToEvade->SetDebugBehaviorEnabled(CanDebugBehavior);

			for (ASteeringAgent* Agent : Agents)
			{
				if (!Agent)
				{
					continue;
				}

				Agent->SetDebugBehaviorEnabled(CanDebugBehavior);
			}
		}


		if (pTrimWorld)
		{
			ImGui::Checkbox("Trim World", &pTrimWorld->bShouldTrimWorld);
		}

		

		ImGui::Text("(Blended) Behavior Weights");
		ImGui::Spacing();

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Cohesion",
			pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal;
			}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Separation",
			pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal;
			}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Alignment",
			pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight = InVal;
			}, "%.2f");


		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Seek",
			pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight = InVal;
			}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Wander",
			pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0.0f, 1.0f,
			[this](const float InVal)
			{
				pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight = InVal;
			}, "%.2f");

		

		ImGui::Text("Radii");
		ImGui::Spacing();
	
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Neighborhood Radius",
			NeighborhoodRadius, 0.0f, 1000.0f,
			[this](const float InVal)
			{
				NeighborhoodRadius = InVal;
			}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter(
			"Evade Radius",
			pEvadeNearbyBehavior->EvadeRadius, 0.0f, 1000.0f,
			[this](const float InVal)
			{
				pEvadeNearbyBehavior->EvadeRadius = InVal;
			}, "%.2f");


		ImGui::Spacing();


		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
	// TODO: Debug Render the neighbors for the first agent in the flock
	if (Agents.IsEmpty())
	{
		return;
	}

	const ASteeringAgent* FirstAgent = Agents[0];
	if (!FirstAgent)
	{
		return;
	}
	

#ifndef GAMEAI_USE_SPACE_PARTITIONING
	RegisterNeighbors(FirstAgent);
#endif

	const auto CurrentNeighbors = GetNeighbors();
	const int CurrentNrAmountOfNeighbors = GetNrOfNeighbors();
	if (CurrentNeighbors.IsEmpty())
	{
		return;
	}
	
	for (const auto Agent : Agents)
	{
		if (!Agent)
		{
			int i = 0;
			for (i = 0; i < Agents.Num(); ++i)
			{
				if (Agents[i] == Agent)
					break;
			}
			
			UE_LOGFMT(LogTemp, Warning, "Agent {Index} is somehow invalid", i);
			continue;
		}
		Agent->SetBodyMaterial(Agent->GetNormalBodyMaterial());
	}

	FirstAgent->SetBodyMaterial(FirstAgent->GetBlueBodyMaterial());

	for (int i{}; i < CurrentNrAmountOfNeighbors; ++i)
	{
		const ASteeringAgent* Neighbor = CurrentNeighbors[i];
		if (!Neighbor || Neighbor == FirstAgent)
		{
			continue;
		}

		Neighbor->SetBodyMaterial(Neighbor->GetHighlightedBodyMaterial());
	}
	
	DrawDebugCircle(pWorld, FVector(FirstAgent->GetPosition(), 0), NeighborhoodRadius, 
		ConstantHelpers::DebugDefaultCircleSegments, FColor::Red, 
		false, 0.f, 0, 5.f, FVector(1,0,0), 
		FVector(0,1,0), false);
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(const ASteeringAgent* const pAgent)
{
	NrOfNeighbors = 0;

	for (const auto OtherAgent : Agents)
	{
		if (pAgent == OtherAgent)
		{
			continue;
		}

		if (!OtherAgent)
		{
			continue;
		}


		const float Distance = FVector2D::Distance(pAgent->GetPosition(), OtherAgent->GetPosition());

		if (Distance > NeighborhoodRadius)
			continue;

		Neighbors[NrOfNeighbors] = OtherAgent;
		++NrOfNeighbors;
	}
}

#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D AvgPosition = FVector2D::ZeroVector;
	int ValidAgents = 0;

	const auto CurrentNeighbors = GetNeighbors(); 
	const int CurrentNrOfNeighbors = GetNrOfNeighbors();
	
	if (CurrentNrOfNeighbors == 0)
	{
		return FVector2D::ZeroVector;
	}
	
	for (int i = 0; i < CurrentNrOfNeighbors; ++i)
	{
		const auto pNeighbor = CurrentNeighbors[i];
		if (!pNeighbor)
		{
			continue;
		}

		++ValidAgents;
		AvgPosition += pNeighbor->GetPosition();
	}

	AvgPosition /= ValidAgents;

	return AvgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	if (NrOfNeighbors == 0)
	{
		return FVector2D::ZeroVector;
	}

	FVector2D AvgVelocity = FVector2D::ZeroVector;
	int ValidAgents = 0;

#ifndef GAMEAI_USE_SPACE_PARTITIONING
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		const auto Neighbor = Neighbors[i];
		if (!Neighbor)
		{
			continue;
		}

		++ValidAgents;
		AvgVelocity += FVector2D(Neighbor->GetVelocity().X, Neighbor->GetVelocity().Y);
	}

	AvgVelocity /= ValidAgents;
#endif

	return AvgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target) const
{
	pSeekBehavior->SetTarget(Target);
}
