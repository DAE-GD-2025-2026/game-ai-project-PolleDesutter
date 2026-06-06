// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <memory>

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "FSMComponent.generated.h"

namespace GameAI::FSM
{
	class State;
	class Transition;
	class FSM;
}

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEAIPROG_API UFSMComponent : public UBrainComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFSMComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartLogic() override;
	virtual void StopLogic(const FString& Reason) override;

	virtual void ResumeLogic();
	virtual void SuspendLogic();


	virtual bool IsRunning() const override;
	virtual bool IsPaused() const override;

	void AddState(std::unique_ptr<GameAI::FSM::State>&& NewState) const;
	void AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To,
	                   const std::function<bool(UBlackboardComponent*)>& EvalFunc) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	std::unique_ptr<GameAI::FSM::FSM> FSMInstance;
	bool bIsRunning{};
};
