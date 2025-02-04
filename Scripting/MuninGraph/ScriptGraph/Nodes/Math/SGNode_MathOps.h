#pragma once
#include "ScriptGraph/ScriptGraphNode.h"
BeginScriptGraphNode(SGNode_MathAdd)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Add"; }
	std::string GetDescription() const override { return "Calculates the addition between two float values."; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathMull)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Mul"; }
	std::string GetDescription()const override { return "Calculates the multiplication between two float values."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};

BeginScriptGraphNode(SGNode_MathSub)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Sub"; }
	std::string GetDescription()const override { return "Calculates the subtraction between two float values."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};
BeginScriptGraphNode(SGNode_MathCos)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Cos"; }
	std::string GetDescription()const override { return "Calculates the cos float values."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};
BeginScriptGraphNode(SGNode_MathSin)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Sin"; }
	std::string GetDescription()const override { return "Calculates the sin float values."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};

BeginScriptGraphNode(SGNode_MathTan)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Tan"; }
	std::string GetDescription()const override { return "Calculates the tan float values."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};
BeginScriptGraphNode(SGNode_MathLength)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Length"; }
	std::string GetDescription()const override { return "Calculates the length of vector."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};

BeginScriptGraphNode(SGNode_MathDistance)
{
public:
	void Init()override;
	std::string GetNodeTitle() const override { return "Distance"; }
	std::string GetDescription()const override { return "Calculates the distance beetween two vectors."; }
	std::string GetNodeCategory()const override { return "Math"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
};