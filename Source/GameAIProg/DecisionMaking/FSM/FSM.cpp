#include "FSM.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/SPIRV-Headers/include/spirv/unified1/spirv.h>

#include "State.h"
#include "BehaviorTree/BlackboardComponent.h"


GameAI::FSM::FSM::FSM(UBlackboardComponent* NewBlackboardComponent, std::unique_ptr<State>&& StartState) :
	BlackboardComponent{NewBlackboardComponent}
{
	State* StatePointer = StartState.get();
	AddState(std::move(StartState));

	CurrentState = StatePointer;
}

void GameAI::FSM::FSM::Start()
{
	if (bIsRunning)
	{
		return;
	}

	bIsRunning = true;
	CurrentState->OnEnter(BlackboardComponent);
}

void GameAI::FSM::FSM::Resume() const
{
	if (!bIsRunning || !bIsSuspended)
	{
		return;
	}

	CurrentState->OnResume();
}

void GameAI::FSM::FSM::Suspend() const
{
	if (!bIsRunning || bIsSuspended)
	{
		return;
	}

	CurrentState->OnSuspense();
}

void GameAI::FSM::FSM::Stop()
{
	if (!bIsRunning)
	{
		return;
	}

	CurrentState->OnExit();
	CurrentState = nullptr;

	bIsRunning = false;
}

void GameAI::FSM::FSM::Update(const float DeltaTime)
{
	if (!bIsRunning || bIsSuspended)
	{
		return;
	}

	// If No Transition Is Connected to the CurrentState, 
	// then do not check the transitions
	if (!Transitions.contains(CurrentState))
	{
		CurrentState->Update(DeltaTime);
		return;
	}

	TransitionsType CurrentStateTransitions = Transitions.find(CurrentState)->second;

	for (auto [EvalFunc, NewState] : CurrentStateTransitions)
	{
		if (EvalFunc(BlackboardComponent))
		{
			ChangeState(NewState);
			return;
		}
	}
}

void GameAI::FSM::FSM::AddState(std::unique_ptr<State>&& NewState)
{
	States.insert(std::move(NewState));
}

void GameAI::FSM::FSM::AddTransition(State* From, State* To, std::function<bool(UBlackboardComponent*)> EvalFunc)
{
	Transitions[From].push_back({EvalFunc, To});
}

bool GameAI::FSM::FSM::IsRunning() const
{
	return bIsRunning;
}

bool GameAI::FSM::FSM::IsSuspended() const
{
	return bIsSuspended;
}

void GameAI::FSM::FSM::ChangeState(State* NewState)
{
	if (!bIsRunning || bIsSuspended)
	{
		return;
	}

	if (CurrentState)
	{
		CurrentState->OnExit();
	}

	const auto StateIt = std::ranges::find_if(States, [NewState](const std::unique_ptr<State>& State)
	{
		return State.get() == NewState;
	});
	if (StateIt == States.end())
	{
		return;
	}


	CurrentState = NewState;

	if (CurrentState)
	{
		CurrentState->OnEnter(BlackboardComponent);
	}
}
