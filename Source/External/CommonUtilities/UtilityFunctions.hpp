#pragma once
#include <cassert>

namespace CommonUtilities
{
	template<typename T>
	T Max(T a, T b)
	{
		if (a > b)
		{
			return a;
		}
		else
		{
			return b;
		}
	}

	template<typename T>
	T Min(T a, T b)
	{
		if (a < b)
		{
			return a;
		}
		else
		{
			return b;
		}
	}

	template<typename T>
	T Abs(T aValue)
	{
		if (aValue < 0)
		{
			return -aValue;
		}
		else
		{
			return aValue;
		}
	}

	template<typename T>
	T Clamp(T aValue, T aMin, T aMax)
	{
		if (aValue < aMin)
		{
			return aMin;
		}
		else if (aValue > aMax)
		{
			return aMax;
		}
		else
		{
			return aValue;
		}
	}

	template<typename T>
	T Lerp(T startValue, T endValue, float t)
	{
		T lerpValue = endValue - startValue;

		t = Clamp<float>(t, 0, 1);

		return startValue + (lerpValue * t);
	}

	


	template<typename T>
	T Smoothstep(T startValue, T endValue, float t)
	{
		T smoothValue = endValue - startValue;

		t = Clamp<float>(t, 0, 1);
		t = t * t * (3.0f - 2.0f * t);

		return startValue + (smoothValue * t) ;
	}


	template<typename T>
	void Swap(T& a, T& b)
	{
		T tempA = a;
		T tempB = b;

		a = tempB;
		b = tempA;
	}
}