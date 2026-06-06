#pragma once

#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class ChaseState : public FSM
	{
	public:
		virtual void OnEnter(UBlackboardComponent* NewBlackboardComponent) override;
		virtual void OnExit() override;
		virtual void OnResume() override;
		virtual void OnSuspense() override;
		virtual void Update() override;
	};
}
