#pragma once
#include <chrono>
#include <ratio>
#include <ctime>

namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		static void Update();

		static float GetDeltaTime();
		static double GetTotalTime();

	private:
		static std::chrono::high_resolution_clock::time_point lastFrameTimePoint;
		static std::chrono::duration<double> myTotalTime;
		static std::chrono::duration<float> myFrameTime;

		inline static bool myInited = false;

		static void Init();
	};
}


