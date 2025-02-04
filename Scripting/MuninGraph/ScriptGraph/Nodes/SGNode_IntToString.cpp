#include "MuninGraph.pch.h"
#include "SGNode_IntToString.h"

#include "ScriptGraph/Types/RegisterScriptGraphTypes.h"

void SGNode_IntToString::Init()
{
	CreateDataPin<int>("Value", PinDirection::Input);
	CreateDataPin<std::string>("Text", PinDirection::Output);
}

size_t SGNode_IntToString::DoOperation()
{
	int val =0;
	if (GetPinData("Value", val))
	{
		SetPinData("Text", std::to_string(val));
		return Exit();
	}
	return 0;
}
