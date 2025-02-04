﻿#include "MuninGraph.pch.h"
#include "ScriptGraphSchema.h"

#include <fstream>

#include "ScriptGraph.h"
#include "ScriptGraphNode.h"
#include "ScriptGraphTypes.h"

#include "json.hpp"
#include "ScriptGraphVariable.h"
#include "Nodes/SGNode_Variable.h"
#include "Nodes/Events/SGNode_EventBeginPlay.h"
#include "Nodes/Events/SGNode_EventTick.h"
#include "Nodes/SGNode_Branch.h"


// Macro for easily registering node types with this Schema.
#define RegisterNodeType(T) { ScriptGraphNodeType type = ScriptGraphNodeType::Create<T>(); nodeTypes.insert({ #T, std::move(type) });}

void ScriptGraphNodeClass::SetupNodeType(ScriptGraphNodeClass& aNodeType, std::shared_ptr<ScriptGraphNode>&& aTempNode)
{
	// We can't create the CDO here since it might cause static init issues.
	// Instead we just make a pointer and fetch data from that.

	//if(!aNodeType.DefaultObject)
	//{
	//	aNodeType.DefaultObject = aNodeType.New();
	//	aNodeType.DefaultObject->Init();
	//}

	const auto uuidObj = AsGUIDAwareSharedPtr(aTempNode);
	aNodeType.TypeName = uuidObj->GetTypeName();
	aNodeType.NodeTitle = aTempNode->GetNodeTitle();
	aNodeType.InternalOnly = aTempNode->IsInternalOnly();
}

std::unordered_map<std::string, const ScriptGraphNodeClass> ScriptGraphSchema::RegisterNodeTypes()
{
	std::unordered_map<std::string, const ScriptGraphNodeClass> nodeTypes;
	//#include "ScriptGraphNodes.inl"
	return std::move(nodeTypes);
}

std::unordered_map<std::string, std::string> ScriptGraphSchema::GetNodeNames()
{
	std::unordered_map<std::string, std::string> namesToTypes;

	for(const auto& [ type, nodeType ] : MyNodeTypesMap())
	{
		const std::shared_ptr<ScriptGraphNode> tempNode = nodeType.New();
		if(!tempNode->IsInternalOnly())
		{
			namesToTypes.insert({ tempNode->GetNodeTitle(), type });
		}
	}

	return namesToTypes;
}

std::shared_ptr<ScriptGraph> ScriptGraphSchema::CreateScriptGraphInternal(bool createEmpty)
{
	CreateNodeCDOs();

	std::shared_ptr<ScriptGraph> aGraph = std::make_shared<ScriptGraph>();

	if (!createEmpty)
	{
		// This is intended to demonstrate an example script graph simulating an Object Blueprint
		// in Unreal Engine. Objects in UE can Tick and have a Begin Play event which fires when
		// the object is spawned. They can also contain other events such as End Play and various
		// other things depending on what type of object it is. I.e. a Pawn, which can be controlled
		// by the player, has the events OnPossessed and OnUnpossessed, for example. A Level blueprint
		// cannot Tick and has no BeginPlay since it always exists when loaded, and instead has other
		// events for connecting players, etc.
		// For simplicity and scalability it would be a good idea to create your own, unique event nodes
		// depending on object type so that you can create a specific ScriptGraph for them. Perhaps
		// even subclass ScriptGraph and modify its ctor to create nodes specific to that graph type.
		const std::unique_ptr<ScriptGraphSchema> graphSchema = aGraph->GetGraphSchema();
		graphSchema->AddEntryNode<SGNode_EventBeginPlay>("BeginPlay");
		graphSchema->AddEntryNode<SGNode_EventTick>("Tick");
	}

	return aGraph;
}

void ScriptGraphSchema::CreateNodeCDOs()
{
	for (auto& [type, nodeType] : MyNodeTypesMap())
	{
		if (!nodeType.DefaultObject)
		{
			nodeType.DefaultObject = nodeType.New();
			nodeType.DefaultObject->Init();
		}
	}
}

std::shared_ptr<ScriptGraph> ScriptGraphSchema::CreateScriptGraph()
{
	return CreateScriptGraphInternal(false);
}

std::shared_ptr<ScriptGraph> ScriptGraphSchema::CreateScriptGraph(const std::shared_ptr<ScriptGraph>& aGraph)
{
	// Time to bake a graph!
	std::string serializedGraph;
	ScriptGraphSchema::SerializeScriptGraph(aGraph, serializedGraph);

	std::shared_ptr<ScriptGraph> result = ScriptGraphSchema::CreateScriptGraphInternal(true);
	ScriptGraphSchema::DeserializeScriptGraph(result, serializedGraph);

	return result;
}

bool ScriptGraphSchema::SerializeScriptGraph(const std::shared_ptr<ScriptGraph>& aGraph, std::string& outResult)
{
	using namespace nlohmann;

	json graphJson;

	graphJson["name"] = "ScriptGraph";

	graphJson["variables"] = json::array();
	for(auto& [varName, variable] : aGraph->myVariables)
	{
		json varJson;
		varJson["name"] = varName;
		varJson["type"] = variable->GetTypeData()->GetTypeName();

		const std::type_index varTypeId = variable->GetTypeData()->GetType();

		std::vector<uint8_t> varData;
		ScriptGraphDataTypeRegistry::Serialize(varTypeId, variable->DefaultData.Ptr, varData);
		varJson["value"] = varData;

		graphJson["variables"].push_back(varJson);
	}

	graphJson["nodes"] = json::array();

	for(const auto& [nodeUID, node] : aGraph->myNodes)
	{
		// GlobalUID keeps track of the type, which is handy.
		const auto uuidAwareBase = AsGUIDAwarePtr(node.get());

		json nodeJson;

		// We save the type of node, the UID (will be unique in the scope of
		// the file we save this graph to, but nowhere else), and the node pins.
		nodeJson["type"] = uuidAwareBase->GetTypeName();
		nodeJson["UID"] = uuidAwareBase->GetUID();
		nodeJson["entryHandle"] = node->IsEntryNode() ? aGraph->myNodeUIDToEntryHandle.find(uuidAwareBase->GetUID())->second : "";

		nodeJson["x"] = node->myPosition[0];
		nodeJson["y"] = node->myPosition[1];
		nodeJson["z"] = node->myPosition[2];

		if(const std::shared_ptr<VariableNodeBase> variableNode = std::dynamic_pointer_cast<VariableNodeBase>(node))
		{
			nodeJson["variable"] = variableNode->myVariable->Name;
		}

		nodeJson["pins"] = json::array();

		// We only store Data pins since we only need to keep track of data values
		// on them at present.
		for (auto& [pinUID, pin] : node->GetPins())
		{
			if(pin.GetType() == ScriptGraphPinType::Data || pin.GetType() == ScriptGraphPinType::Variable)
			{
				json pinJson;
				pinJson["name"] = pin.GetLabel();

				std::vector<uint8_t> pinData;
				pinData.resize(pin.myData.TypeData->GetTypeSize());
				ScriptGraphDataTypeRegistry::Serialize(pin.GetDataType(), pin.myData.Ptr, pinData);
				pinJson["value"] = pinData;

				nodeJson["pins"].push_back(pinJson);
			}		
		}

		graphJson["nodes"].push_back(nodeJson);
	}

	graphJson["edges"] = json::array();

	// For edges we need to know the UID of the involved nodes, same UID we saved
	// in the nodes section above, and we also need to know which pin. Pins are
	// stored using labels on that node making them easy to find.
	for (auto& [edgeUID, edge] : aGraph->myEdges)
	{
		const ScriptGraphPin& sourcePin = aGraph->GetPinFromUID(edge.FromUID);
		const auto sourceNodeUUID = AsGUIDAwareSharedPtr(sourcePin.GetOwner());

		const ScriptGraphPin& targetPin = aGraph->GetPinFromUID(edge.ToUID);
		const auto targetNodeUUID = AsGUIDAwareSharedPtr(targetPin.GetOwner());

		json edgeJson;
		edgeJson["sourceUID"] = sourceNodeUUID->GetUID();
		edgeJson["sourcePin"] = sourcePin.GetLabel();

		edgeJson["targetUID"] = targetNodeUUID->GetUID();
		edgeJson["targetPin"] = targetPin.GetLabel();

		graphJson["edges"].push_back(edgeJson);
	}

	outResult = graphJson.dump();

	return true;
}

bool ScriptGraphSchema::DeserializeScriptGraph(std::shared_ptr<ScriptGraph>& outGraph, const std::string& inData)
{
	outGraph = CreateScriptGraphInternal(true);
	std::unique_ptr<ScriptGraphSchema> outGraphSchema = outGraph->GetGraphSchema();

	using namespace nlohmann;
	json graphJson = json::parse(inData);

	outGraph->myEdges.clear();
	outGraph->myPins.clear();
	outGraph->myEntryPoints.clear();
	outGraph->myNodeUIDToEntryHandle.clear();
	outGraph->myNodes.clear();
	outGraph->myVariables.clear();

	// Load all the variables
	for (const auto& it : graphJson["variables"])
	{
		const std::string varName = it.at("name");
		const std::string varType = it.at("type");
		const std::vector<uint8_t> varData = it.at("value");

		const std::type_index varTypeId = ScriptGraphDataTypeRegistry::GetTypeFromString(varType);

		std::shared_ptr<ScriptGraphVariable> newVariable = std::make_shared<ScriptGraphVariable>();
		newVariable->Data = ScriptGraphDataTypeRegistry::GetDataObjectOfType(varTypeId);
		newVariable->Name = varName;
		newVariable->DefaultData = ScriptGraphDataTypeRegistry::GetDataObjectOfType(varTypeId);

		ScriptGraphDataTypeRegistry::Deserialize(varTypeId, newVariable->DefaultData.Ptr, varData);
		newVariable->ResetVariable();

		outGraph->myVariables.insert({ varName, newVariable });
	}

	std::unordered_map<size_t, std::shared_ptr<ScriptGraphNode>> fileUIDToNode;

	// Create all the nodes.
	for (const auto& it : graphJson["nodes"])
	{
		const std::string nodeType = it.at("type");
		const auto typeData = MyNodeTypesMap().find(nodeType);
		std::shared_ptr<ScriptGraphNode> newNode = typeData->second.New();

		if (std::shared_ptr<VariableNodeBase> varNode = std::dynamic_pointer_cast<VariableNodeBase>(newNode))
		{
			const std::string nodeVarName = it.at("variable");
			std::shared_ptr<ScriptGraphVariable> nodeVar = outGraph->myVariables.find(nodeVarName)->second;
			varNode->SetNodeVariable(nodeVar);

			for (auto& [pinUID, pin] : newNode->myPins)
			{
				if (pin.GetType() == ScriptGraphPinType::Variable)
				{
					pin.InitVariableBlock(nodeVar->Data.TypeData->GetType());
				}
			}
		}

		for (const auto& pinIt : it.at("pins"))
		{
			const std::string pinName = pinIt.at("name");
			const std::vector<uint8_t> varData = pinIt.at("value");
			if (auto pinLbl = newNode->myPinLabelToUID.find(pinName); pinLbl != newNode->myPinLabelToUID.end())
			{
				if (auto pinObj = newNode->myPins.find(pinLbl->second); pinObj != newNode->myPins.end())
				{
					ScriptGraphDataTypeRegistry::Deserialize(pinObj->second.GetDataType(), pinObj->second.myData.Ptr, varData);
					//memcpy_s(pinObj->second.myData.Ptr, pinObj->second.myData.TypeData->GetTypeSize(), varData.data(), varData.size());
				}
			}
		}

		const auto uuidAwareBase = AsGUIDAwareSharedPtr(newNode);

		const float x = it.at("x");
		const float y = it.at("y");
		const float z = it.at("z");
		newNode->myPosition[0] = x;
		newNode->myPosition[1] = y;
		newNode->myPosition[2] = z;

		// Here we should read all the pins and restore their values if we can.
		// This gets fun if we have a lot of custom types to account for :).
		// TODO: read constant pin data. Needs to be saved too. Won't work for pointers of course
		// since they're just an integer.

		if (!outGraphSchema->RegisterNode(newNode))
		{
			assert(false && "The graph has invalid numbers of a node!");
		}

		if (newNode->IsEntryNode())
		{
			outGraphSchema->RegisterEntryPointNode(newNode, it.at("entryHandle"));
		}

		fileUIDToNode.insert({ it.at("UID"), newNode });
	}

	// And all the edges
	for (const auto& it : graphJson["edges"])
	{
		const size_t sourceUID = it.at("sourceUID");
		const std::string sourcePinLabel = it.at("sourcePin");
		const size_t targetUID = it.at("targetUID");
		const std::string targetPinLabel = it.at("targetPin");

		// Find the source node owning the source pin.
		if (auto srcIt = fileUIDToNode.find(sourceUID); srcIt != fileUIDToNode.end())
		{
			// And find the target node owning the target pin.
			if (auto tgtIt = fileUIDToNode.find(targetUID); srcIt != fileUIDToNode.end())
			{
				const std::shared_ptr<ScriptGraphNode> sourceNode = srcIt->second;
				const std::shared_ptr<ScriptGraphNode> targetNode = tgtIt->second;

				// Get the actual pins from the nodes.
				const ScriptGraphPin& srcPin = sourceNode->GetPin(sourcePinLabel);
				const ScriptGraphPin& tgtPin = targetNode->GetPin(targetPinLabel);

				// And create a link.
				outGraphSchema->CreateEdge(srcPin.GetUID(), tgtPin.GetUID());
			}
		}
	}

	return true;
}

ScriptGraphPin& ScriptGraphSchema::GetMutablePin(size_t aPinUID)
{
	return *myGraph->myPins.find(aPinUID)->second;
}

bool ScriptGraphSchema::RegisterNode(std::shared_ptr<ScriptGraphNode> aNode)
{
	assert(aNode && "The requested node is not of the correct base node type!");

	if(aNode->GetPins().empty())
		aNode->Init();

	const auto uidAwareNode = AsGUIDAwarePtr(aNode.get());

	const unsigned maxInstances = aNode->MaxInstancesPerGraph();
	if(maxInstances > 0)
	{
		if (auto countIt = myNodeTypeCounts.find(uidAwareNode->GetTypeName()); countIt != myNodeTypeCounts.end())
		{
			if (maxInstances < countIt->second + 1)
			{
				return false; // No making more copies of this node!
			}

			countIt->second++;
		}
		else
		{
			myNodeTypeCounts.insert({ uidAwareNode->GetTypeName(), 1 });
		}
	}

	aNode->myOwner = myGraph;
	
	myGraph->myNodes.insert({uidAwareNode->GetUID(), aNode});
	for (auto& [key, value] : aNode->myPins)
	{
		myGraph->myPins.insert({ key, &value });
	}

	return true;
}

void ScriptGraphSchema::RegisterEntryPointNode(std::shared_ptr<ScriptGraphNode> aNode, const std::string& aEntryHandle)
{
	myGraph->myEntryPoints.insert({ aEntryHandle, aNode });
	const auto uuidAwareBase = AsGUIDAwarePtr(aNode.get());
	myGraph->myNodeUIDToEntryHandle.insert({ uuidAwareBase->GetUID(), aEntryHandle });
	myGraphEntryPoints.push_back(aEntryHandle);
}

void ScriptGraphSchema::CreateEdgeInternal(ScriptGraphPin& aSourcePin, ScriptGraphPin& aTargetPin) const
{
	const size_t newEdgeUID = myGraph->myNextEdgeId++;
	const float edgeThickness = aSourcePin.GetType() == ScriptGraphPinType::Exec ? 3.0f : 1.0f;

	myGraph->myEdges.insert(
		{
			newEdgeUID,
			{
				newEdgeUID,
				aSourcePin.GetUID(),
				aTargetPin.GetUID(),
				ScriptGraphDataTypeRegistry::GetDataTypeColor(aSourcePin.GetDataType()),
				edgeThickness
			}
		});

	aSourcePin.AddPinEdge(newEdgeUID);
	aTargetPin.AddPinEdge(newEdgeUID);

	assert(aTargetPin.GetNumConnections() == 1);
	assert(aSourcePin.GetNumConnections() >= 1);
}

void ScriptGraphSchema::RegenerateEntryPointList()
{
	myGraphEntryPoints.clear();
	for(const auto& [handle, node] : myGraph->myEntryPoints)
	{
		myGraphEntryPoints.push_back(handle);
	}
}

void ScriptGraphSchema::CheckCyclicLink(const std::shared_ptr<ScriptGraphNode>& aNode,
	const std::shared_ptr<ScriptGraphNode>& aBaseNode, bool& outResult) const
{
	// TODO: This just checks for cyclic nodes but will prevent functionality similar
	// to UEs ForEachWithBreak nodes because you wouldn't be allowed to connect to the
	// break input exec pin. Ideally this should count pins if it's Exec pins we're
	// looking at instead of just looking at nodes. 

	if(outResult)
	{
		return;
	}

	const auto& baseNodeUID = AsGUIDAwareSharedPtr(aBaseNode)->GetUID();
	const auto& pins = aNode->GetPins();
	for(const auto& [pinUID, pin] : pins)
	{
		if(pin.GetPinDirection() == PinDirection::Output)
		{
			const auto& edges = pin.GetEdges();
			for(const size_t edgeUID : edges)
			{
				const NodeGraphEdge& edge = myGraph->GetEdgeFromUID(edgeUID);
				const auto& targetPin = myGraph->GetPinFromUID(edge.ToUID);
				const auto& targetNode = AsGUIDAwareSharedPtr(targetPin.GetOwner());
				if(targetNode->GetUID() == baseNodeUID)
				{
					outResult |= true;
					return;
				}
				CheckCyclicLink(targetPin.GetOwner(), aBaseNode, outResult);
			}
		}
	}
}

ScriptGraphSchema::ScriptGraphSchema(const std::shared_ptr<void>& aGraph)
{
	CreateNodeCDOs();

	myGraph = std::static_pointer_cast<ScriptGraph>(aGraph);
	for(const auto& [nodeUID, node] : myGraph->myNodes)
	{
		const auto uidAwareBase = AsGUIDAwarePtr(node.get());
		if(auto countIt = myNodeTypeCounts.find(uidAwareBase->GetTypeName()); countIt != myNodeTypeCounts.end())
		{
			countIt->second++;
		}
		else
		{
			myNodeTypeCounts.insert({ uidAwareBase->GetTypeName(), 1 });
		}
	}

	RegenerateEntryPointList();
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddNode(const std::string& aType)
{
	if(const auto typeData = MyNodeTypesMap().find(aType); typeData != MyNodeTypesMap().end())
	{
		return AddNode(typeData->second);
	}

	return nullptr;
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddNode(const ScriptGraphNodeClass& aType)
{
	std::shared_ptr<ScriptGraphNode> newNode = aType.New();
	if (RegisterNode(newNode))
	{
		return newNode;
	}

	return nullptr;
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddEntryNode(const std::string& aType,
	const std::string& aEntryHandle)
{
	if (const auto typeData = MyNodeTypesMap().find(aType); typeData != MyNodeTypesMap().end())
	{
		return AddEntryNode(typeData->second, aEntryHandle);
	}

	return nullptr;
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddEntryNode(const ScriptGraphNodeClass& aType,
	const std::string& aEntryHandle)
{
	if (aType.DefaultObject->IsEntryNode())
	{
		if(std::shared_ptr<ScriptGraphNode> result = AddNode(aType))
		{
			RegisterEntryPointNode(result, aEntryHandle);
			return result;
		}
	}

	return nullptr;
}

void ScriptGraphSchema::RemoveNode(size_t aNodeUID)
{
	if(const auto nodeIt = myGraph->myNodes.find(aNodeUID); nodeIt != myGraph->myNodes.end())
	{
		for(auto& [ pinUID, pin ] : nodeIt->second->myPins)
		{
			// Copy edges since that list will change.
			const std::vector<size_t> pinEdges = pin.myEdges;
			for(const size_t edgeUID : pinEdges)
			{
				RemoveEdge(edgeUID);
			}

			myGraph->myPins.erase(pinUID);
		}

		if(nodeIt->second->IsEntryNode())
		{
			const auto uuidAwareNode = AsGUIDAwareSharedPtr(nodeIt->second);
			for (auto entryIt = myGraph->myEntryPoints.begin(); entryIt != myGraph->myEntryPoints.end(); ++entryIt)
			{
				const auto uuidAwarePtr = AsGUIDAwareSharedPtr(entryIt->second);
				if(uuidAwarePtr->GetUUID() == uuidAwareNode->GetUUID())
				{
					myGraph->myNodeUIDToEntryHandle.erase(uuidAwarePtr->GetUID());
					myGraph->myEntryPoints.erase(entryIt);
					break;
				}
			}

			RegenerateEntryPointList();
		}

		const auto uidAwareNode = AsGUIDAwareSharedPtr(nodeIt->second);
		if (const auto countIt = myNodeTypeCounts.find(uidAwareNode->GetTypeName()); countIt != myNodeTypeCounts.end())
		{
			countIt->second--;
			if(countIt->second <= 0)
			{
				myNodeTypeCounts.erase(countIt);
			}
		}

		myGraph->myNodes.erase(nodeIt);
	}
}

void ScriptGraphSchema::RemoveVariable(const std::string& aVariableName)
{
	// Need to plow through all nodes that use this variable and eat them.
	std::vector<size_t> nodeUIDsToDelete;
	for(const auto& [nodeUID, node] : myGraph->myNodes)
	{
		if(std::shared_ptr<VariableNodeBase> varNode = std::dynamic_pointer_cast<VariableNodeBase>(node))
		{
			if(varNode->myVariable->Name == aVariableName)
				nodeUIDsToDelete.push_back(nodeUID);
		}
	}

	for(const size_t& uid : nodeUIDsToDelete)
	{
		RemoveNode(uid);
	}

	myGraph->myVariables.erase(aVariableName);
}

std::shared_ptr<SGNode_GetVariable> ScriptGraphSchema::AddGetVariableNode(const std::string& aVariableName)
{
	auto varIt = myGraph->myVariables.find(aVariableName);
	assert(varIt != myGraph->myVariables.end() && "No such variable!");

	std::shared_ptr<SGNode_GetVariable> node = std::make_shared<SGNode_GetVariable>();
	node->Init();
	node->SetNodeVariable(varIt->second);

	for(auto& [pinUID, pin] : node->myPins)
	{
		if(pin.GetType() == ScriptGraphPinType::Variable)
		{
			pin.InitVariableBlock(varIt->second->Data.TypeData->GetType());
		}
	}

	RegisterNode(node);

	return node;
}

std::shared_ptr<SGNode_SetVariable> ScriptGraphSchema::AddSetVariableNode(const std::string& aVariableName)
{
	auto varIt = myGraph->myVariables.find(aVariableName);
	assert(varIt != myGraph->myVariables.end() && "No such variable!");

	std::shared_ptr<SGNode_SetVariable> node = std::make_shared<SGNode_SetVariable>();
	node->Init();
	node->SetNodeVariable(varIt->second);

	for (auto& [pinUID, pin] : node->myPins)
	{
		if (pin.GetType() == ScriptGraphPinType::Variable)
		{
			pin.InitVariableBlock(varIt->second->Data.TypeData->GetType());
		}
	}

	RegisterNode(node);

	return node;
}

bool ScriptGraphSchema::CanCreateEdge(size_t aSourcePinUID, size_t aTargetPinUID, std::string& outMesssage) const
{
	if(aSourcePinUID == aTargetPinUID)
	{
		return false;
	}

	const ScriptGraphPin& pinA = myGraph->GetPinFromUID(aSourcePinUID);
	const ScriptGraphPin& pinB = myGraph->GetPinFromUID(aTargetPinUID);

	const ScriptGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
	const ScriptGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

	if(sourcePin.GetUID() == targetPin.GetUID())
	{
		outMesssage = "Cannot link a pin to itself.";
		return false;
	}

	const auto sourceNode = AsGUIDAwareSharedPtr(sourcePin.GetOwner());
	const auto targetNode = AsGUIDAwareSharedPtr(targetPin.GetOwner());
	if(sourceNode->GetUID() == targetNode->GetUID())
	{
		outMesssage = "Cannot connect a pin to another pin on the same node!";
		return false;
	}

	if(sourcePin.GetPinDirection() == targetPin.GetPinDirection())
	{
		outMesssage = "Cannot link a pin to one of the same direction.";
		return false;
	}

	bool isCyclic = false;
	CheckCyclicLink(targetPin.GetOwner(), sourcePin.GetOwner(), isCyclic);
	if(isCyclic)
	{
		outMesssage = "This connection would result in a cyclic graph loop!";
		return false;
	}

	if(sourcePin.GetType() == ScriptGraphPinType::Exec && targetPin.GetType() == ScriptGraphPinType::Exec)
	{
		return true;
	}

	if (sourcePin.GetType() != ScriptGraphPinType::Exec && targetPin.GetType() != ScriptGraphPinType::Exec)
	{
		if(sourcePin.GetDataType() != targetPin.GetDataType())
		{
			outMesssage = std::string(sourcePin.GetDataType().name()) + " is not compatible with " + std::string(targetPin.GetDataType().name());
			return false;
		}

		return true;
	}

	return false;
}

bool ScriptGraphSchema::CreateEdge(size_t aSourcePinUID, size_t aTargetPinUID)
{
	std::string outMessage;
	if(CanCreateEdge(aSourcePinUID, aTargetPinUID, outMessage))
	{
		ScriptGraphPin& pinA = GetMutablePin(aSourcePinUID);
		ScriptGraphPin& pinB = GetMutablePin(aTargetPinUID);
		
		ScriptGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
		ScriptGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

		// Exec can have only one connection right now. Remove all connections from the pin.
		if(sourcePin.GetType() == ScriptGraphPinType::Exec && sourcePin.IsPinConnected())
		{
			DisconnectPin(sourcePin.GetUID());
		}

		if(targetPin.IsPinConnected())
		{
			DisconnectPin(targetPin.GetUID());
		}

		CreateEdgeInternal(sourcePin, targetPin);

		return true;
	}

	return false;
}

bool ScriptGraphSchema::DisconnectPin(size_t aPinUID)
{
	ScriptGraphPin& pin = GetMutablePin(aPinUID);

	const std::vector<size_t> pinEdgeIds = pin.GetEdges();
	for(auto it = pinEdgeIds.begin(); it != pinEdgeIds.end(); ++it)
	{
		RemoveEdge(*it);
	}

	return true;
}

bool ScriptGraphSchema::RemoveEdge(size_t aEdgeUID)
{	
	auto it = myGraph->myEdges.find(aEdgeUID);
	assert(it != myGraph->myEdges.end());
		
	ScriptGraphPin& fromPin = GetMutablePin(it->second.FromUID);
	ScriptGraphPin& toPin = GetMutablePin(it->second.ToUID);

	fromPin.RemovePinEdge(aEdgeUID);
	toPin.RemovePinEdge(aEdgeUID);

	it = myGraph->myEdges.erase(it);

	return true;
}

void ScriptGraphSchema::UpdateNodePositionCache(const std::shared_ptr<ScriptGraphNode>& aNode, float x, float y, float z)
{
	aNode->myPosition[0] = x;
	aNode->myPosition[1] = y;
	aNode->myPosition[2] = z;
}

void ScriptGraphSchema::GetNodePositionCache(const std::shared_ptr<ScriptGraphNode>& aNode, float& outX, float& outY,
	float& outZ)
{
	outX = aNode->myPosition[0];
	outY = aNode->myPosition[1];
	outZ = aNode->myPosition[2];
}
