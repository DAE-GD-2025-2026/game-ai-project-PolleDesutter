#pragma once

#include <functional>
#include <memory>
#include <set>


class UBlackboardComponent;

namespace GameAI::FSM
{
	class State;

	class FSM
	{
	public:
		// Pointer and not reference because I allow a nullptr to be passed
		explicit FSM(UBlackboardComponent* NewBlackboardComponent);
		virtual ~FSM() = default;

		virtual void Update(float DeltaTime);

		void AddState(const std::unique_ptr<State>&& NewState);
		void AddTransition(State* From, State* To, std::function<bool()> EvalFunc);

	protected:
		void ChangeState(State* NewState);

		UBlackboardComponent* BlackboardComponent;
		State* CurrentState{};

		// First State Is Key to (if) CurrentState, and an expression & NewState is the other State
		std::unordered_map<State*, std::pair<State*, std::function<bool()>>> Transitions{};
		std::set<std::unique_ptr<State>> States;
	};
}
