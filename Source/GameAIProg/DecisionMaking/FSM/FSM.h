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
		void AddTransition(State* From, State* To, std::function<bool(UBlackboardComponent*)> EvalFunc);

	protected:
		void ChangeState(State* NewState);

		UBlackboardComponent* BlackboardComponent;
		State* CurrentState{};

		// If Function Returns True, State* is the state to transition to
		using TransitionsType = std::vector<std::pair<std::function<bool(UBlackboardComponent*)>, State*>>;

		// First State is the Current State or a possible Current State
		std::unordered_map<State*, std::pair<State*, TransitionsType>> Transitions;
		std::set<std::unique_ptr<State>> States;
	};
}
