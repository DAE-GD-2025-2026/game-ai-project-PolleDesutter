#pragma once

// class UBlackboardComponent;
#include "BrainComponent.h"

namespace GameAI::FSM
{
	class State
	{
	public:
		State() = default;
		virtual ~State() = default;

		virtual void OnEnter(UBlackboardComponent* NewBlackboardComponent);

		virtual void OnExit() = 0;
		virtual void OnResume() = 0;
		virtual void OnSuspense() = 0;

		virtual void Update(float DeltaTime) = 0;

	private:
		UBlackboardComponent* BlackboardComponent{};
	};
}
