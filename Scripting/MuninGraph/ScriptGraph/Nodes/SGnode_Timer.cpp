#include "MuninGraph.pch.h"
#include "SGnode_Timer.h"
#include <thread>
void SGNode_Timer::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateExecPin("On Timer", PinDirection::Output, false);

	CreateDataPin<int>("Duration", PinDirection::Input);

}

size_t SGNode_Timer::DoOperation()
{
	int Duration;
	
	ExitViaPin("Out");
	if (GetPinData("Duration", Duration) )
	{
		std::thread TimerThread([this, Duration]()
			{
				std::this_thread::sleep_for(std::chrono::seconds(Duration));
				return ExitViaPin("On Timer");
			});
		TimerThread.detach();
	
	}
	return 0;
}