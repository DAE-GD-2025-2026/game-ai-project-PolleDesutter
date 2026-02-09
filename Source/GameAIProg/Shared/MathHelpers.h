#pragma once


namespace MathHelpers
{
	
#pragma region EaseIn
	
	FORCEINLINE float EaseInCubic(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		return FMath::Pow(T, 3);
	}
	
	FORCEINLINE float EaseInCirc(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		return 1 - FMath::Sqrt(1 - FMath::Pow(T, 2));
	}

	FORCEINLINE float EaseInQuad(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		return FMath::Pow(T, 2);
	}
	
	FORCEINLINE float EaseInQuart(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		return FMath::Pow(T, 4);
	}
	
	FORCEINLINE float EaseInQuint(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		return FMath::Pow(T, 5);
	}
	
	FORCEINLINE float EaseInExpo(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		if (T == 0.f)
		{
			return 0.f;
		}
		
		return FMath::Pow(2, 10 * T - 10);
	}
	
	
	
#pragma endregion 
	
	
	
#pragma region EaseOut
	
	FORCEINLINE float EaseOutCubic(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
	
		return 1.f - FMath::Pow(1.f - T, 3);
	}
	
	FORCEINLINE float EaseOutQuad(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		return 1 - (1 - T) * (1 - T);
	}
	
	
#pragma endregion 
	
	
#pragma region EaseInOut
	
	FORCEINLINE float EaseInOutCubic(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		if (T < 0.5f)
		{
			return 4 * FMath::Pow(T, 3);
		}
		else
		{
			return 1 - FMath::Pow(-2 * T + 2, 3) / 2.f;	
		}
	}
	
	FORCEINLINE float EaseInOutExpo(float T)
	{
		T = FMath::Clamp(T, 0.0f, 1.0f);
		
		if (T == 0.f)
		{
			return 0.f;
		}
		
		if (T == 1.f)
		{
			return 1.f;
		}
		
		if (T < 0.5f)
		{
			return FMath::Pow(2, 20 * T - 10) / 2.f;		
		}
		else
		{
			return (2 - FMath::Pow(2, -20 * T + 10)) / 2.f;		
		}
	}
	
	
#pragma endregion 
	
	
	
	
	
	
}
