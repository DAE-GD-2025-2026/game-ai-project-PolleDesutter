#pragma once

namespace GameAI::HeuristicFunctions
{
	// Common typedef
	typedef float(*Heuristic)(float, float);
	
	// Manhattan distance
	static float Manhattan(float X, float Y)
	{
		return X + Y;
	}

	// Euclidean distance
	static float Euclidean(float X, float Y)
	{
		return sqrtf(X * X + Y * Y);
	}

	// Sqrt Euclidean distance
	static float SqEuclidean(float X, float Y)
	{
		return X * X + Y * Y;
	}

	// Octile distance
	static float Octile(float X, float Y)
	{
		float constexpr f = 0.414213562373095048801f; // == sqrt(2) - 1;
		return X < Y ? f * X + Y : f * Y + X;
	}

	// Chebyshev distance
	static float Chebyshev(float X, float Y)
	{
		return std::max(X, Y);
	}
};
