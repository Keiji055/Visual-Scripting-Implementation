#pragma once
#include "MuninScriptGraph.h"
#include <string>
#include "ScriptGraph/ScriptGraphNode.h"
#include "GameObjectRegistery.h"
#include <memory>
#include "CommonUtilities\InputHandler.h"
#include <ScriptGraph/Nodes/Events/SGNode_EventBase.h>
// Example file for how to register nodes in another project, i.e. not inside MuninGraph.lib.
// To get all the nodes inside MuninGraph in your project to auto-register you need to link
// with /WHOLEARCHIVE:MuninGraph.lib or VS will optimize the auto reg away. Similarly if you
// have this file in a lib of your own you need to /WHOLEARCHIVE that file too. If this lives
// in an EXE project then it'll work as intended on its own.

// NOTE THE INCLUDE!

BeginScriptGraphNode(MVNode_TestNode)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Model Viewer Test Node"; }
	size_t DoOperation() override;
};
#pragma once

BeginScriptGraphNode(SGNode_CreateGameObject)
{
public:
	void Init() override;
	std::string GetNodeTitle()const override { return "CreateGO"; }
	std::string GetDescription()const override { return "Creates a gameobject"; }
	std::string GetNodeCategory()const override { return "GameObejcts"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }
private:
	std::shared_ptr<GameObject> myGameObject;
};
BeginScriptGraphNode(SGNode_GetGOPos)
{
public:
	void Init() override;
	std::string GetNodeTitle()const override { return "GetPos"; }
	std::string GetDescription()const override { return "Returns the vector of the gameobject"; }
	std::string GetNodeCategory()const override { return "GameObejcts"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }

};

BeginScriptGraphNode(SGNode_SetGOPos)
{
public:
	void Init() override;
	std::string GetNodeTitle()const override { return "SetPos"; }
	std::string GetDescription()const override { return "Sets the vector of the gameobject"; }
	std::string GetNodeCategory()const override { return "GameObejcts"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }

};

BeginScriptGraphNode(SGNode_UpdateGOPos)
{
public:
	void Init() override;
	std::string GetNodeTitle()const override { return "UpdatesGO"; }
	std::string GetDescription()const override { return "Updates all gameObjects"; }
	std::string GetNodeCategory()const override { return "GameObejcts"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }

};

BeginScriptGraphNode(SGNode_UpdateGOScale)
{
public:
	void Init() override;
	std::string GetNodeTitle()const override { return "SetGOScale"; }
	std::string GetDescription()const override { return "Updates scale for gameObjects"; }
	std::string GetNodeCategory()const override { return "GameObejcts"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return false; }

};

BeginScriptGraphNode(SGNode_GetMousePos)
{
public:
		void Init() override;
	std::string GetNodeTitle()const override { return "MousePos"; }
	std::string GetDescription()const override { return "GetsCurentMousePos"; }
	std::string GetNodeCategory()const override { return "Input"; }
	size_t DoOperation()override;
	bool IsSimpleNode()const override { return true; }
};
BeginScriptGraphDerivedNode(SGNode_OnTriggerEntry, SGNode_EventBase)
{
public:

	void Init() override;
	FORCEINLINE bool IsEntryNode() const override { return true; }
	FORCEINLINE bool IsInternalOnly() const override { return false; }
	std::string GetNodeTitle() const override { return "On Trigger Entry"; }
	std::string GetDescription() const override { return "An event that fires when a hitbox has been entried "; };
	std::string GetNodeCategory() const override { return "Event"; }
};

BeginScriptGraphDerivedNode(SGNode_OnTriggerExit, SGNode_EventBase)
{
public:

	void Init() override;
	FORCEINLINE bool IsEntryNode() const override { return true; }
	FORCEINLINE bool IsInternalOnly() const override { return false; }
	std::string GetNodeTitle() const override { return "On Trigger Exit"; }
	std::string GetDescription() const override { return "An event that fires when a hitbox has been exited "; };
	std::string GetNodeCategory() const override { return "Event"; }
};

