#pragma once
#include "Kismet/GameplayStatics.h"
#include <type_traits>

#include "Components/BoxComponent.h"


namespace UnrealHelpers
{
	template <typename T>	
	concept DerivedFromAActor = std::is_base_of_v<AActor, T>;
	
	template <DerivedFromAActor T> 
	static T* GetFirstActorOfClass(const UObject* WorldContextObject);
	
	[[nodiscard]] bool IsPositionInsideVolume(const UBoxComponent& BoxComponent, const FVector& Position);

	
}



template <UnrealHelpers::DerivedFromAActor T>
T* UnrealHelpers::GetFirstActorOfClass(const UObject* WorldContextObject)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, T::StaticClass(), Actors);
	
	if (Actors.IsEmpty())
	{
		return nullptr;
	}
	
	return Cast<T>(Actors[0]);
}

