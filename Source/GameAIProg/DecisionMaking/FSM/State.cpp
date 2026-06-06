#include "FSM.h"


void GameAI::FSM::State::OnEnter(UBlackboardComponent* NewBlackboardComponent)
{
	BlackboardComponent = NewBlackboardComponent;
	if (!BlackboardComponent)
	{
		UE_LOGFMT(LogTemp, Warning, "FSM::OnEnter: NewBlackboardComponent is invalid");
	}
}
