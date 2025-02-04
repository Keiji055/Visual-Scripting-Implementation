#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_BranchIF)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Condition"; }
	std::string GetDescription() const override { return "Returns the condition"; }
	std::string GetNodeCategory() const override { return "Branch"; }
	size_t DoOperation() override;
	FORCEINLINE bool IsSimpleNode() const override { return true; }
};
