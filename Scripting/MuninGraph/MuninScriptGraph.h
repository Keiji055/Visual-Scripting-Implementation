#pragma once
#include "MuninGraph.h"
#include "ScriptGraph/ScriptGraphPin.h"
#include "ScriptGraph/ScriptGraphNode.h"
#include "ScriptGraph/ScriptGraph.h"
#include "ScriptGraph/ScriptGraphSchema.h"

namespace __MuninScriptGraphInternal
{
	extern bool MuninScriptGraphInit();
	static bool IsMuninScriptGraphRegistered = MuninScriptGraphInit();
}