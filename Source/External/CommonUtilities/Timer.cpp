#include "pch.h"
#include "Timer.h"

std::chrono::high_resolution_clock::time_point CommonUtilities::Timer::lastFrameTimePoint;
std::chrono::duration<double> CommonUtilities::Timer::myTotalTime;
std::chrono::duration<float> CommonUtilities::Timer::myFrameTime;

CommonUtilities::Timer::Timer()
{

}

void CommonUtilities::Timer::Update()
{
	Init();
	std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

	myFrameTime = now - lastFrameTimePoint;
	myTotalTime += now - lastFrameTimePoint;

	lastFrameTimePoint = now;
}

float CommonUtilities::Timer::GetDeltaTime()
{
	return myFrameTime.count();
}

double CommonUtilities::Timer::GetTotalTime()
{
	return myTotalTime.count();
}

void CommonUtilities::Timer::Init()
{
	if (myInited)
	{
		return;
	}
	myTotalTime = myTotalTime.zero();
	myFrameTime = myFrameTime.zero();

	lastFrameTimePoint = std::chrono::high_resolution_clock::now();
	myInited = true;
}