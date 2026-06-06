#include "FSM.h"

#include "State.h"
#include "BehaviorTree/BlackboardComponent.h"

GameAI::FSM::FSM::FSM(UBlackboardComponent* NewBlackboardComponent, std::unique_ptr<State>&& StartState) :
	BlackboardComponent{NewBlackboardComponent}
{
	State* StatePointer = StartState.get();
	AddState(std::move(StartState));

	ChangeState(StatePointer);
}

void GameAI::FSM::FSM::Update(const float DeltaTime)
{
	// If No Transitions is connected to CurrentState
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

void GameAI::FSM::FSM::ChangeState(State* NewState)
{
	if (CurrentState)
	{
		CurrentState->OnExit();
	}

	CurrentState = NewState;

	if (CurrentState)
	{
		CurrentState->OnEnter(BlackboardComponent);
	}
}
