// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMComponent.h"

#include "FSM.h"
#include "State.h"


// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UFSMComponent::~UFSMComponent()
{
}

void UFSMComponent::InitializeFSM(std::unique_ptr<GameAI::FSM::State>&& StartState)
{
	FSMInstance = std::make_unique<GameAI::FSM::FSM>(BlackboardComp, std::move(StartState));
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FSMInstance->Update(DeltaTime);
}

void UFSMComponent::StartLogic()
{
	Super::StartLogic();

	FSMInstance->Start();
}

void UFSMComponent::StopLogic(const FString& Reason)
{
	Super::StopLogic(Reason);

	FSMInstance->Stop();
}


void UFSMComponent::ResumeLogic()
{
	// I know the BrainComponent Has a Resume/Pause Logic, but they are
	// special, so I cannot be assed to work with them
	FSMInstance->Resume();
}

void UFSMComponent::SuspendLogic()
{
	FSMInstance->Suspend();
}

bool UFSMComponent::IsRunning() const
{
	return FSMInstance->IsRunning();
}

bool UFSMComponent::IsPaused() const
{
	return FSMInstance->IsSuspended();
}

void UFSMComponent::AddState(std::unique_ptr<GameAI::FSM::State>&& NewState) const
{
	FSMInstance->AddState(std::move(NewState));
}

void UFSMComponent::AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To,
                                  const std::function<bool(UBlackboardComponent*)>& EvalFunc) const
{
	FSMInstance->AddTransition(From, To, EvalFunc);
}

// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}
