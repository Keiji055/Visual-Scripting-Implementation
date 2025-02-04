#pragma once
#include "ScriptGraph/ScriptGraphNode.h"
BeginScriptGraphNode(SGNode_Timer)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Executes when timmer is finished"; }
	std::string GetDescription() const override { return "Executes when timmer is finished"; }
	std::string GetNodeCategory() const override { return "Timer"; }
	size_t DoOperation() override;
	FORCEINLINE bool IsSimpleNode() const override { return false; }
};