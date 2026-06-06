#pragma once

#include "DecisionMaking/FSM/State.h"

namespace GameAI::FSM
{
	class ChaseState : public State
	{
	public:
		virtual void OnEnter(UBlackboardComponent* NewBlackboardComponent) override;
		virtual void OnExit() override;
		virtual void OnResume() override;
		virtual void OnSuspense() override;

		virtual void Update(float DeltaTime) override;
	};
}
