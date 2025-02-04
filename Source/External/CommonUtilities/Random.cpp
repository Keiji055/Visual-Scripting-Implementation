#include "pch.h"
#include "Random.h"

namespace CommonUtilities
{
	void Random::Init()
	{
		myRandomEngine = std::mt19937(std::random_device()());
		myIsInitialized = true;
	}

	float Random::GetRandomFloat(float aMin, float aMax)
	{
		CheckInitialized();

		if (aMin > aMax)
		{
			return 0.f;
		}

		std::uniform_real_distribution<float> distribution(aMin, aMax);
		return distribution(myRandomEngine);
	};

	int Random::GetRandomInt(int aMin, int aMax)
	{
		CheckInitialized();

		if (aMin > aMax)
		{
			return 0;
		}

		std::uniform_int_distribution<int> distribution(aMin, aMax);
		return distribution(myRandomEngine);
	}

	bool Random::GetRandomBool()
	{
		CheckInitialized();

		std::uniform_int_distribution<int> distribution(0, 1);
		return distribution(myRandomEngine);
	}

	int32_t Random::GetRandomInt()
	{
		CheckInitialized();

		std::uniform_int_distribution<int32_t> distribution(INT32_MIN, INT32_MAX);
		return distribution(myRandomEngine);
	}

	uint32_t Random::GetRandomUInt()
	{
		CheckInitialized();

		std::uniform_int_distribution<int32_t> distribution(0, UINT32_MAX);
		return distribution(myRandomEngine);
	}

	int64_t Random::GetRandomInt64()
	{
		CheckInitialized();

		std::uniform_int_distribution<int64_t> distribution(INT64_MIN, INT64_MAX);
		return distribution(myRandomEngine);
	}

	int64_t Random::GetRandomInt64(int64_t aMin, int64_t aMax)
	{
		CheckInitialized();

		std::uniform_int_distribution<int64_t> distribution(aMin, aMax);
		return distribution(myRandomEngine);
	}

	uint64_t Random::GetRandomUInt64()
	{
		CheckInitialized();

		std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
		return distribution(myRandomEngine);
	}

	void Random::CheckInitialized()
	{
		if (!myIsInitialized)
		{
			Init();
		}
	}
}
