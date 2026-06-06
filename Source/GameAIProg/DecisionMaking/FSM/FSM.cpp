#include "FSM.h"

#include "State.h"

GameAI::FSM::FSM::FSM(UBlackboardComponent* NewBlackboardComponent) :
	BlackboardComponent{NewBlackboardComponent}
{
}

void GameAI::FSM::FSM::Update(const float DeltaTime)
{
	// If No Transitions is connected to CurrentState
	if (!Transitions.contains(CurrentState))
	{
		CurrentState->Update(DeltaTime);
		return;
	}

	auto StateTransitionIt = Transitions.find(CurrentState);
}

void GameAI::FSM::FSM::AddState(const std::unique_ptr<State>&& NewState)
{
	States.insert(NewState);
}

void GameAI::FSM::FSM::AddTransition(State* From, State* To, std::function<bool(UBlackboardComponent*)> EvalFunc)
{
	Transitions[From].second.push_back(std::make_pair(EvalFunc, To));
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
