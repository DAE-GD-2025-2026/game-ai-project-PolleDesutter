#pragma once

// class UBlackboardComponent;
#include "BrainComponent.h"

namespace GameAI::FSM
{
	class FSM
	{
	public:
		FSM() = default;
		virtual ~FSM() = default;
	
		virtual void OnEnter(UBlackboardComponent* NewBlackboardComponent);
	
		virtual void OnExit() = 0;
		virtual void OnResume() = 0;
		virtual void OnSuspense() = 0;
	
		virtual void Update() = 0;
	
	private:
		UBlackboardComponent* BlackboardComponent{};
	
	};
}
