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
		explicit FSM(UBlackboardComponent* NewBlackboardComponent, std::unique_ptr<State>&& StartState);
		virtual ~FSM() = default;

		virtual void Start();
		virtual void Resume() const;
		virtual void Suspend() const;
		virtual void Stop();

		virtual void Update(float DeltaTime);


		virtual void AddState(std::unique_ptr<State>&& NewState);
		virtual void AddTransition(State* From, State* To, std::function<bool(UBlackboardComponent*)> EvalFunc);

		// TODO: Add a GetState or something similar 

		virtual bool IsRunning() const;
		virtual bool IsSuspended() const;

	protected:
		virtual void ChangeState(State* NewState);

		UBlackboardComponent* BlackboardComponent;
		State* CurrentState{};

		// If Function Returns True, State* is the state to transition to
		using TransitionsType = std::vector<std::pair<std::function<bool(UBlackboardComponent*)>, State*>>;

		// First State is the Current State or a possible Current State
		std::unordered_map<State*, TransitionsType> Transitions;
		std::set<std::unique_ptr<State>> States;

		bool bIsRunning{};
		bool bIsSuspended{};
	};
}
