#pragma once
#include <string>
#include <string_view>

#include "Kismet/GameplayStatics.h"
#include <type_traits>

#include "Components/BoxComponent.h"
#include "Containers/StringConv.h"


namespace UnrealHelpers
{
	template <typename T>	
	concept DerivedFromAActor = std::is_base_of_v<AActor, T>;
	
	template <DerivedFromAActor T> 
	static T* GetFirstActorOfClass(const UObject* WorldContextObject);
	
	[[nodiscard]] bool IsPositionInsideVolume(const UBoxComponent& BoxComponent, const FVector& Position);
	
	[[nodiscard]] bool GetMouseWorldPosition(const UObject* WorldContextObject, const APlayerController* PlayerController, FVector& OutPosition);

	
	[[nodiscard]] inline FString ConvertStringToFString(const std::string& String)
	{
	    return FString(UTF8_TO_TCHAR(String.c_str()), String.length());
	}
	
	[[nodiscard]] inline FString ConvertStringViewToFString(const std::string_view& StringView)
	{
		return ConvertStringToFString(std::string(StringView));
	}
	
	inline FString GetClassNameFromFunction(const char* CharString);
	inline FString GetFunctionNameFromFunction(const char* CharString);
	
	void SpawnAndAttachTextToActor(const UObject* WorldContextObject, AActor* Actor);
	 
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



FString UnrealHelpers::GetClassNameFromFunction(const char* CharString)
{
	const FString String = ANSI_TO_TCHAR(CharString);
	const int32 SeparatorIndex = String.Find("::");
		
	if (SeparatorIndex == INDEX_NONE)
	{
		return String;
	}
		
	return String.Left(SeparatorIndex);
}

FString UnrealHelpers::GetFunctionNameFromFunction(const char* CharString)
{
	const FString String = ANSI_TO_TCHAR(CharString);
	int32 SeparatorIndex = String.Find("::");
		
	if (SeparatorIndex == INDEX_NONE)
	{
		return String;
	}
		
	if (SeparatorIndex != 0)
	{
		// TODO: figure out how this string index (FString::Right) works
		// I don't know how this works, but cannot be assed to 
		// figure it out right now
			
		// e.g. TestFunction 
		///  SeparatorIndex => estFunction
		// --SeparatorIndex => stFunction
		// ++SeparatorIndex => TestFunction
			
		++SeparatorIndex;
	}
		
	return String.Right(SeparatorIndex);
}

