#include "MuninGraph.pch.h"
#include "SGNode_Branch.h"
void SGNode_BranchIF::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("True", PinDirection::Output, false);
	CreateExecPin("False", PinDirection::Output, false);

	CreateDataPin<bool>("Condition", PinDirection::Input);

}

size_t SGNode_BranchIF::DoOperation()
{
	bool condition;
	if (GetPinData("Condition",condition))
	{
		if (condition)
		{
			return ExitViaPin("True");
		}
		else
		{
			return ExitViaPin("False");
		}
	}
	return 0;
}