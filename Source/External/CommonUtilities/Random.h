#pragma once
#include <random>

namespace CommonUtilities
{
	class Random
	{
	public:
		Random() = delete;
		~Random() = delete;

		static void Init();

		static float GetRandomFloat(float aMin, float aMax);
		static int GetRandomInt(int aMin, int aMax);

		static bool GetRandomBool();

		static int32_t GetRandomInt();
		static uint32_t GetRandomUInt();

		static int64_t GetRandomInt64();
		static int64_t GetRandomInt64(int64_t aMin, int64_t aMax);
		static uint64_t GetRandomUInt64();

	private:
		inline static bool myIsInitialized = false;
		inline static thread_local std::mt19937 myRandomEngine;

		static void CheckInitialized();
	};
}
