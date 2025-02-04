#pragma once
#include <memory>
#include <vector>
#include "d3d11.h"
#include "imgui_node_editor.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
namespace ImNodeEd = ax::NodeEditor;
#include "MuninScriptGraph.h"
#include "GameObjectRegistery.h"

namespace Tga
{
	class Texture;
};

class ScriptGraphEditor
{
	struct SearchMenuItem
	{
		std::string Title;
		std::string Value;
		size_t Rank = SIZE_MAX;
		std::string Tag;
	};

	struct ContextMenuItem
	{
		std::string Title;
		std::string Value;
		std::string Tag;
	};

	struct ContextMenuCategory
	{
		std::string Title;
		std::vector<ContextMenuItem> Items;
	};

	struct EditorInterfaceState
	{
		// The current value of the node search list.
		std::string bgCtxtSearchField;
		bool bgCtxtSearchFieldChanged = false;
		bool bgCtxtSearchFocus = false;
		std::vector<SearchMenuItem> bgCtxtSearchFieldResults;

		std::string varNewVarNameField;
		int varNewVarTypeIdx = 0;
		ScriptGraphDataObject varNewVarValue;
		int varInlineEditingIdx = -1;

		ContextMenuCategory bgCtxtVariablesCategory;
		std::string varToDelete;

		bool flowShowFlow = false;
		bool isTicking = false;

		bool errorIsErrorState = false;
		std::string errorMessage;
		size_t errorNodeId = 0;

		bool initNavToContent = false;
	} myState;

	// Actual storage of looking up categories when generating.
	static inline std::unordered_map<std::string, ContextMenuCategory> myBgContextCategories;
	// Sorted list of context category names.
	static inline std::vector<std::string> myBgContextCategoryNamesList;

	std::unique_ptr<ScriptGraphSchema> mySchema;

	Tga::Texture* myNodeHeaderTexture;
	Tga::Texture* myGetterGradient;
	std::unordered_map<ScriptGraphNodeType, Tga::Texture*> myNodeTypeIcons;

	// TEMP
	std::shared_ptr<ScriptGraph> myGraph;

	void UpdateVariableContextMenu();

	// Context Menues
	void BackgroundContextMenu();
	void NodeContextMenu(size_t aNodeUID);
	void LinkContextMenu(size_t aLinkUID);

	// Modals
	void TriggerEntryPoint();
	void EditVariables();

	// Event Stuff
	void HandleEditorCreate();
	void HandleEditorDelete();

	// Graph Rendering stuff
	void RenderNode(const ScriptGraphNode* aNode);
	void RenderPin(const ScriptGraphPin* aPin);
	void DrawPinIcon(const ScriptGraphPin* aPin, ImVec4 aPinColor, bool isConnected) const;

	void HandleScriptGraphError(const ScriptGraph& aGraph, size_t aNodeUID, const std::string& anErrorMessage);

	void ClearErrorState();

	void LoadTextureFromMemory(Tga::Texture* outTexture, const std::wstring& aName, const unsigned char* someImageData, size_t anImageDataSize, const D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc);

	void DuplicateSelectedNodes();
	void SaveSelectedNodes();
	void PasteDuplicateNodes();

public:
	std::vector<ImNodeEd::NodeId>  mySelectedNodesSave;
	void Init();
	void Update(float aDeltaTime);
	void Render();
};
