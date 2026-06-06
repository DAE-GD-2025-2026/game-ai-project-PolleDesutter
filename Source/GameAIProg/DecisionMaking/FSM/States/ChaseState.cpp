#include "ChaseState.h"

void GameAI::FSM::ChaseState::OnEnter(UBlackboardComponent* NewBlackboardComponent)
{
	FSM::OnEnter(NewBlackboardComponent);
	
	UE_LOGFMT(LogTemp, Warning, "COOOL");
}

void GameAI::FSM::ChaseState::OnExit()
{
}

void GameAI::FSM::ChaseState::OnResume()
{
}

void GameAI::FSM::ChaseState::OnSuspense()
{
}

void GameAI::FSM::ChaseState::Update()
{
}
