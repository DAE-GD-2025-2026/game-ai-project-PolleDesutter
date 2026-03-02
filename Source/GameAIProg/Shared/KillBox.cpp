// Fill out your copyright notice in the Description page of Project Settings.


#include "KillBox.h"

#include "Components/BoxComponent.h"


// Sets default values
AKillBox::AKillBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AKillBox::BeginPlay()
{
	Super::BeginPlay();
	
	// UBoxComponent
	
}

