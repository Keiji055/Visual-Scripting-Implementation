﻿#pragma once
#include <array>

#include "ScriptGraphPin.h"
#include "../Graph/NodeGraphNode.h"
#include "ScriptGraph.h"
#include "ScriptGraphSchema.h"

class ScriptGraph;

struct ScriptGraphNodePayload
{
	friend class ScriptGraphNode;

private:

	std::unordered_map<std::string, ScriptGraphDataObject> Data;

public:

	template<typename T>
	void SetVariable(const std::string& aString, const T& aValue)
	{
		if(const auto it = Data.find(aString); it != Data.end())
		{
			if(it->second.TypeData->GetType() != typeid(T))
			{
				ScriptGraphDataObject::Create<T>(it->second);
			}
			it->second.SetData(aValue);
		}
		else
		{
			Data.insert({ aString, ScriptGraphDataObject::Create(aValue) });
		}
	}

	template<typename T>
	bool TryGetVariable(const std::string& aString, T& outValue) const
	{
		if (const auto it = Data.find(aString); it != Data.end())
		{
			if(it->second.TypeData->GetType() == typeid(T))
			{
				outValue = it->second.GetData<T>();
				return true;
			}
		}

		return false;
	}
};

class ScriptGraphNode : public NodeGraphNode<ScriptGraphPin, ScriptGraph, ScriptGraphSchema>, public std::enable_shared_from_this<ScriptGraphNode>
{
	typedef NodeGraphNode<ScriptGraphPin, ScriptGraph, ScriptGraphSchema> ParentClass;

	bool isExecNode = false;

	std::string myErrorMessage;
	bool hasErrored = false;

protected:

	template<typename DataType>
	void CreateDataPin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(ScriptGraphPin::CreateDataPin<DataType>(AsSharedPtr(), aLabel, ScriptGraphPinType::Data, PinIcon::Circle, aDirection, hideLabelOnNode));
	}

	void CreateVariablePin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(ScriptGraphPin::CreatePin(AsSharedPtr(), aLabel, ScriptGraphPinType::Variable, PinIcon::Circle, aDirection, hideLabelOnNode));
	}

	void CreateExecPin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(ScriptGraphPin::CreatePin(AsSharedPtr(), aLabel, ScriptGraphPinType::Exec, PinIcon::Exec, aDirection, hideLabelOnNode));
		isExecNode = true;
	}

	virtual size_t Exit();
	virtual size_t ExitViaPin(const std::string& aPinLabel);

	size_t ExitWithError(const std::string& anErrorMessage);

	std::shared_ptr<ScriptGraphNode> AsSharedPtr() { return shared_from_this(); }

public:

	/**
	 * Called when this node is asked to Execute. This will ONLY happen if the node has at least one Exec Input Pin.
	 * By default this function will call DoOperation on the node but you may modify this behavior as you please.
	 * @param anEntryPinUID The Pin that caused this node to Execute (i.e. an Input Exec pin we have). If we have no input Exec pins this will be 0!
	 * @returns The Pin Index of the pin we want to exit on after performing our operation or 0 if we don't have a pin to exit on.
	 */
	virtual size_t Exec(size_t anEntryPinUID);

	virtual size_t DoOperation() { return Exit(); }

	virtual void DeliverPayload(const ScriptGraphNodePayload& aPayload);

	virtual ~ScriptGraphNode() override = default;

	FORCEINLINE bool IsExecNode() const { return isExecNode; }
	FORCEINLINE virtual bool IsEntryNode() const { return false; }
	FORCEINLINE virtual bool IsInternalOnly() const { return false; }
	FORCEINLINE virtual bool IsDebugOnly() const { return false; }

	// If True, this node will draw without a header if it has no Exec Pins.
	FORCEINLINE virtual bool IsSimpleNode() const { return true; }

	FORCEINLINE virtual std::string GetDescription() const { return "A ScriptGraph Node."; }

	virtual FORCEINLINE GraphColor GetNodeHeaderColor() const { return GraphColor(80, 124, 153, 255); };

	// Controls how many instances of this node may coexist in the same graph.
	// If > 0 this will be enforced.
	FORCEINLINE virtual unsigned MaxInstancesPerGraph() const { return 0; }

	FORCEINLINE virtual ScriptGraphNodeType GetNodeType() const { return ScriptGraphNodeType::Undefined; }

	FORCEINLINE bool HasError() const { return hasErrored; }
	FORCEINLINE const std::string& GetErrorMessage() const { return myErrorMessage; }
};

template<typename N>
struct RegisterScriptNode
{
	static inline bool IsRegisteredSchema = ScriptGraphSchema::RegisterNodeType<N>();
	static inline bool IsRegisteredGraph = ScriptGraph::RegisterNodeType<N>();
};

template<typename N, typename B>
struct RegisterScriptNodeWithBase
{
	static inline bool IsRegisteredSchema = ScriptGraphSchema::RegisterNodeTypeWithBase<N, B>();
	static inline bool IsRegisteredGraph = ScriptGraph::RegisterNodeTypeWithBase<N, B>();
};

// Declare a run of the mill Script Graph Node
#define BeginScriptGraphNode(T) class T : public ScriptGraphNode, public RegisterScriptNode<T>, public Munin::ObjectGUID<T>

// Declare a node that is intended to work as base for other Script Graph Nodes
#define BeginScriptGraphBaseNode(T) class T : public ScriptGraphNode

// Declare a node that inherits from a node declared with BeginScriptGraphBaseNode(T).
#define BeginScriptGraphDerivedNode(T, B) class T : public B, public RegisterScriptNodeWithBase<T, B>, public Munin::ObjectGUID<T>