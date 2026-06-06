#include "FSM.h"

#include "State.h"

GameAI::FSM::FSM::FSM(UBlackboardComponent* NewBlackboardComponent) :
	BlackboardComponent{NewBlackboardComponent}
{
}

void GameAI::FSM::FSM::Update(float DeltaTime)
{
	CurrentState->Update(DeltaTime);
}

void GameAI::FSM::FSM::AddState(const std::unique_ptr<State>&& NewState)
{
	States.insert(NewState);
}

void GameAI::FSM::FSM::AddTransition(State* From, State* To, std::function<bool()> EvalFunc)
{
	Transitions[From] = {To, EvalFunc};
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
