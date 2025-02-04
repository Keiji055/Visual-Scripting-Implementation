#include "MuninGraph.pch.h"
#include "SGNode_MathOps.h"

void SGNode_MathAdd::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathAdd::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA + inB;
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}

	return 0;
}

void SGNode_MathSub::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathSub::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA - inB;
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathMull::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathMull::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA * inB;
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathCos::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathCos::DoOperation()
{
	float inA = 0;

	if (GetPinData("A", inA))
	{
		const float result = cos(inA);
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathSin::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathSin::DoOperation()
{
	float inA = 0;

	if (GetPinData("A", inA))
	{
		const float result = sin(inA);
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathTan::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathTan::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = atan2(inA, inB);
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathLength::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("X", PinDirection::Input);
	CreateDataPin<float>("Y", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathLength::DoOperation()
{
	float x = 0;
	float y = 0;

	if (GetPinData("X", x) && GetPinData("Y", y))
	{
		const float result = (x * x) + (y * y);
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}


void SGNode_MathDistance::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("X 1", PinDirection::Input);
	CreateDataPin<float>("Y 1", PinDirection::Input);

	CreateDataPin<float>("X 2", PinDirection::Input);
	CreateDataPin<float>("Y 2", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathDistance::DoOperation()
{
	float x = 0;
	float y = 0;
	float x2 = 0;
	float y2 = 0;

	if (GetPinData("X 1", x) && GetPinData("Y 1", y) && GetPinData("X 2", x2) && GetPinData("Y 2", y2))
	{
		float x3 = x2 - x;
		float y3 = y2 - y;
		float resulttemp = (x3 * x3) + (y3 * y3);
		const float result = std::sqrt(resulttemp);
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}
	return 0;
}