#pragma once

// Used by macro's
#include "ConversionHelpers.h"
#include "UnrealHelpers.h"


#define CLASS_NAME		ConversionHelpers::SplitStringView(std::string_view(__FUNCTION__), "::")[0]
#define FUNCTION_NAME	ConversionHelpers::SplitStringView(std::string_view(__FUNCTION__), "::")[1]

#define CLASS_NAME_FSTRING UnrealHelpers::GetClassNameFromFunction(__FUNCTION__)
#define FUNCTION_NAME_FSTRING UnrealHelpers::GetFunctionNameFromFunction(__FUNCTION__)


namespace ConstantHelpers
{
	constexpr float DebugDefaultLineLength = 150.f;
	constexpr float	DebugDefaultPointSize = 10.f ;
	constexpr int	DebugDefaultCircleSegments = 12 ;
	
	static FColor DebugDirectionColor = FColor::Magenta;
	static FColor DebugDirectionLeftColor = FColor::Cyan;
	
	static FColor DebugTargetColor = FColor::Red; 

	
}

