#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_IntToString)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Int to String"; }
	std::string GetDescription() const override { return "Prints a provided text string to stdout."; }
	std::string GetNodeCategory() const override { return "Casts"; }
	size_t DoOperation() override;
	FORCEINLINE bool IsSimpleNode() const override { return true; }
};
