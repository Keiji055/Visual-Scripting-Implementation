﻿#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "ScriptGraphEditor.h"
#include "tge/EngineDefines.h"
#include "CommonUtilities\InputHandler.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "Event_Icon.h"

#include "imgui_internal.h"

#include "misc/cpp/imgui_stdlib.h"
#include "ScriptGraph/ScriptGraphTypes.h"
#include "ScriptGraph/Nodes/SGNode_Variable.h"
#include "ScriptGraph/Nodes/Math/SGNode_MathOps.h"
#include "ScriptGraph/Nodes/SGnode_Timer.h"
#include "ScriptGraph/Nodes/Events/SGNode_EventBeginPlay.h"

#include "RegisterExternalTypes.h"
#include "ScriptGraphNodeGradient.h"
#include "Function_Icon.h"
#include "GetGradient.h"
#include "ScriptGraph/Nodes/Events/SGNode_EventBase.h"

#include "tge/render/RenderCommon.h"
#include "tge/graphics/DX11.h"

#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>

#include <DDSTextureLoader/DDSTextureLoader11.h>
#include <tge/texture/texture.h>

namespace ImNodeEd = ax::NodeEditor;

//#pragma region ImGui Operators
//
//inline ImVec2 operator+(const ImVec2& A, const ImVec2& B)
//{
//	return { A.x + B.x, A.y + B.y };
//}
//
//inline ImVec2 operator-(const ImVec2& A, const ImVec2& B)
//{
//	return { A.x - B.x, A.y - B.y };
//}
//
//inline ImVec2 operator*(const ImVec2& A, const ImVec2& B)
//{
//	return { A.x * B.x, A.y * B.y };
//}
//
//inline ImVec2 operator*(const ImVec2& A, const float S)
//{
//	return { A.x * S, A.y * S };
//}
//
//inline ImVec4 operator/(const ImVec4& A, const float S)
//{
//	return { A.x / S, A.y / S, A.z / S, A.w / S };
//}
//
//#pragma endregion

// Copied from MuninUtils.
// Measures differences in two strings.
template<typename T>
std::enable_if_t<std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, typename T::size_type>
LevenshteinDistance(const T& aString, const T& aSearchString)
{
	if(aString.size() > aSearchString.size())
	{
		return LevenshteinDistance(aSearchString, aString);
	}

	using TSize = typename T::size_type;
	const TSize minSize = aString.size();
	const TSize maxSize = aSearchString.size();
	std::vector<TSize> levenDistance(minSize + 1);

	for(TSize s = 0; s <= minSize; ++s)
	{
		levenDistance[s] = s;
	}

	for(TSize s = 1; s <= maxSize; ++s)
	{
		TSize lastDiag = levenDistance[0], lastDiagMem;
		++levenDistance[0];

		for(TSize t = 1; t <= minSize; ++t)
		{
			lastDiagMem = levenDistance[t];
			if(aString[t - 1] == aSearchString[s - 1])
			{
				levenDistance[t] = lastDiag;
			}
			else
			{
				levenDistance[t] = std::min(std::min(levenDistance[t - 1], levenDistance[t]), lastDiag) + 1;
			}
			lastDiag = lastDiagMem;
		}
	}

	return levenDistance[minSize];
}

ImNodeEd::Config nodeEditorCfg;
ImNodeEd::EditorContext* nodeEditorContext;

void ScriptGraphEditor::UpdateVariableContextMenu()
{
	myState.bgCtxtVariablesCategory.Items.clear();
	myState.bgCtxtVariablesCategory.Title = "Variables";
	const ScriptGraphNodeClass& GetNodeType = ScriptGraphSchema::GetNodeTypeByClass<SGNode_GetVariable>();
	const ScriptGraphNodeClass& SetNodeType = ScriptGraphSchema::GetNodeTypeByClass<SGNode_SetVariable>();
	const auto getNodeUUID = AsGUIDAwareSharedPtr(GetNodeType.DefaultObject);
	const auto setNodeUUID = AsGUIDAwareSharedPtr(SetNodeType.DefaultObject);
	for (const auto& [varId, var] : mySchema->GetVariables())
	{

		myState.bgCtxtVariablesCategory.Items.push_back({ "Get " + var->Name, getNodeUUID->GetTypeName(), varId });
		myState.bgCtxtVariablesCategory.Items.push_back({ "Set " + var->Name, setNodeUUID->GetTypeName(), varId });
	}
}

void ScriptGraphEditor::BackgroundContextMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("BackgroundContextMenu"))
	{
		//ImGui::PushItemWidth(100.0f);
		myState.bgCtxtSearchFieldChanged = ImGui::InputText("##nodeSearch", &myState.bgCtxtSearchField);
		if(myState.bgCtxtSearchFocus)
		{
			ImGui::SetKeyboardFocusHere(0);
			myState.bgCtxtSearchFocus = false;
		}
		//ImGui::PopItemWidth();

		if(myState.bgCtxtSearchField.empty())
		{
			for (const std::string& catName : myBgContextCategoryNamesList)
			{
				const auto catIt = myBgContextCategories.find(catName);
				if (ImGui::TreeNodeEx(catName.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					for (const auto& item : catIt->second.Items)
					{
						if (ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
						{
							if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							{
								const ScriptGraphNodeClass& type = ScriptGraphSchema::GetSupportedNodeTypes().find(item.Value)->second;
								std::shared_ptr<ScriptGraphNode> newNode;
								if (type.DefaultObject->IsEntryNode())
								{
									newNode = mySchema->AddEntryNode(type, item.Title);
								}
								else
								{
									newNode = mySchema->AddNode(type);
								}
								const auto uuidAwareNewNode = AsGUIDAwareSharedPtr(newNode);
								const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());								
								ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
								ImGui::CloseCurrentPopup();
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}

			if(ImGui::TreeNodeEx(myState.bgCtxtVariablesCategory.Title.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
			{
				for(const auto& item : myState.bgCtxtVariablesCategory.Items)
				{
					if (ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						{
							const ScriptGraphNodeClass& type = ScriptGraphSchema::GetSupportedNodeTypes().find(item.Value)->second;

							std::shared_ptr<ScriptGraphNode> newNode;
							if (type.Type == typeid(SGNode_GetVariable))
								newNode = mySchema->AddGetVariableNode(item.Tag);
							else
								newNode = mySchema->AddSetVariableNode(item.Tag);

							const auto uuidAwareNewNode = AsGUIDAwareSharedPtr(newNode);
							const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
							ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
							ImGui::CloseCurrentPopup();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		else
		{
			if(myState.bgCtxtSearchFieldChanged)
			{
				myState.bgCtxtSearchFieldResults.clear();
				const auto supportedNodeTypes = ScriptGraphSchema::GetSupportedNodeTypes();
				for (const auto& [nodeName, nodeType] : supportedNodeTypes)
				{
					if (nodeType.DefaultObject->IsInternalOnly())
						continue;

					myState.bgCtxtSearchFieldResults.push_back({
						nodeType.DefaultObject->GetNodeTitle(),
						nodeType.TypeName,
						0,
						""
						});

					myState.bgCtxtSearchFieldResults.back().Rank = LevenshteinDistance(myState.bgCtxtSearchFieldResults.back().Title, myState.bgCtxtSearchField);
				}

				for(const auto& varItem : myState.bgCtxtVariablesCategory.Items)
				{
					myState.bgCtxtSearchFieldResults.push_back({
						varItem.Title,
						varItem.Value,
						0,
						varItem.Tag
						});

					myState.bgCtxtSearchFieldResults.back().Rank = LevenshteinDistance(myState.bgCtxtSearchFieldResults.back().Title, myState.bgCtxtSearchField);
				}

				std::sort(myState.bgCtxtSearchFieldResults.begin(), myState.bgCtxtSearchFieldResults.end(),
					[](const SearchMenuItem& A, const SearchMenuItem& B)
					{
						return A.Rank < B.Rank;
					});
			}

			for(auto& item : myState.bgCtxtSearchFieldResults)
			{
				if (ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						const ScriptGraphNodeClass& type = ScriptGraphSchema::GetSupportedNodeTypes().find(item.Value)->second;
						std::shared_ptr<ScriptGraphNode> newNode = nullptr;
						
						if(type.Type == typeid(SGNode_GetVariable))
						{
							newNode = mySchema->AddGetVariableNode(item.Tag);
						}
						else if (type.Type == typeid(SGNode_SetVariable))
						{
							newNode = mySchema->AddSetVariableNode(item.Tag);
						}
						else
						{
							if(type.DefaultObject->IsEntryNode())
							{
								newNode = mySchema->AddEntryNode(type, item.Title);
							}
							else
							{
								newNode = mySchema->AddNode(type);
							}							
						}
						
						const auto uuidAwareNewNode = AsGUIDAwareSharedPtr(newNode);
						const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
						ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
						myState.bgCtxtSearchFieldChanged = false;
						myState.bgCtxtSearchField.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::TreePop();
				}
			}
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void ScriptGraphEditor::NodeContextMenu(size_t aNodeUID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("NodeContextMenu"))
	{
		if (ImGui::BeginMenu("Node Context Menu"))
		{
			if (ImGui::MenuItem("Herp")) {}
			if (ImGui::MenuItem("Derp")) {}
			if (ImGui::MenuItem("Slerp")) {}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void ScriptGraphEditor::LinkContextMenu(size_t aLinkUID)
{

}

void ScriptGraphEditor::TriggerEntryPoint()
{
	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::SetNextWindowSize({512, 256});
	if(ImGui::BeginPopupModal("TriggerEntryPoint"))
	{
		const std::vector<std::string>& graphEntryPoints = mySchema->GetEntryPoints();
		ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp);
		for (const auto& entry : graphEntryPoints)
		{
			// Don't trigger the Tick node.
			if(entry != "Tick")
			{
				ImGui::TableNextColumn();
				if (ImGui::Button(entry.c_str()))
				{
					ImGui::CloseCurrentPopup();
					myState.flowShowFlow = true;
					ClearErrorState();
					myGraph->Run(entry);
				}
			}
		}
		ImGui::EndTable();

		if(ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void ScriptGraphEditor::EditVariables()
{
	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::SetNextWindowSize({ 1024, 512 });
	if (ImGui::BeginPopupModal("Edit Variables", 0, ImGuiWindowFlags_NoResize))
	{
		const auto& graphVariables = mySchema->GetVariables();
		ImGui::BeginTable("body", 4, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg);
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("Default");
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();
		int varIdx = 0;
		for(const auto& [ varName, variable ] : graphVariables)
		{
			ImGui::TableNextColumn();
			const GraphColor typeColor = ScriptGraphDataTypeRegistry::GetDataTypeColor(variable->GetTypeData()->GetType()).AsNormalized();
			ImGui::TextColored({ typeColor.R, typeColor.G, typeColor.B, typeColor.A }, varName.c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(ScriptGraphDataTypeRegistry::GetFriendlyName(variable->GetTypeData()->GetType()).c_str());
			ImGui::TableNextColumn();
			if(myState.varInlineEditingIdx == varIdx && ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(variable->GetTypeData()->GetType()))
			{
				ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(variable->GetTypeData()->GetType(), variable->DefaultData.GetUUID(), variable->DefaultData.Ptr);
			}
			else
			{
				ImGui::TextUnformatted(ScriptGraphDataTypeRegistry::GetString(variable->GetTypeData()->GetType(), variable->DefaultData.Ptr).c_str());
			}			
			ImGui::TableNextColumn();
			ImGui::PushID(variable->Name.c_str());
			ImGui::BeginDisabled(myState.varInlineEditingIdx == varIdx);
			if(ImGui::Button("Delete"))
			{
				myState.varToDelete = variable->Name;
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::BeginDisabled(!ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(variable->GetTypeData()->GetType()));
			if(myState.varInlineEditingIdx == varIdx)
			{
				if(ImGui::Button("Save"))
				{
					variable->ResetVariable();
					myState.varInlineEditingIdx = -1;
				}
			}
			else
			{
				if (ImGui::Button("Edit"))
				{
					myState.varInlineEditingIdx = varIdx;
				}
			}
			ImGui::EndDisabled();
			ImGui::PopID();
			varIdx++;
		}
		ImGui::EndTable();

		ImGui::Separator();

		ImGui::TextUnformatted("Create New Variable");

		ImGui::PushItemWidth(200);
		ImGui::InputText("##newVar", &myState.varNewVarNameField);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		const std::vector<std::string>& typeNames = ScriptGraphDataTypeRegistry::GetRegisteredTypeNames();
		ImGui::PushItemWidth(150);
		if(ImGui::BeginCombo("##newVarType", myState.varNewVarTypeIdx >= 0 ? typeNames[myState.varNewVarTypeIdx].c_str() : nullptr, 0))
		{
			for(size_t i = 0; i < typeNames.size(); i++)
			{
				const bool isSelected = (myState.varNewVarTypeIdx == static_cast<int>(i));
				if (ImGui::Selectable(typeNames[i].c_str(), isSelected))
				{
					myState.varNewVarTypeIdx = static_cast<int>(i);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::BeginDisabled(myState.varNewVarNameField.empty() || myState.varNewVarTypeIdx < 0);

		if(!myState.varNewVarValue.TypeData || myState.varNewVarValue.TypeData != ScriptGraphDataTypeRegistry::GetTypeFromFriendlyName(typeNames[myState.varNewVarTypeIdx]))
		{
			myState.varNewVarValue = ScriptGraphDataTypeRegistry::GetDataObjectOfType(*ScriptGraphDataTypeRegistry::GetTypeFromFriendlyName(typeNames[myState.varNewVarTypeIdx]));
		}

		ImGui::TextUnformatted("Default Value:");
		ImGui::SameLine();

		if(ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(*myState.varNewVarValue.TypeData))
		{
			const float y = ImGui::GetCursorPosY(); ImGui::SetCursorPosY(y + 2);
			ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(*myState.varNewVarValue.TypeData, myState.varNewVarValue.GetUUID(), myState.varNewVarValue.Ptr);
			ImGui::SetCursorPosY(y);
			ImGui::SameLine();
		}

		if(ImGui::Button("Create", ImVec2(100, 0)))
		{
			mySchema->AddVariable(myState.varNewVarNameField, myState.varNewVarValue);
			myState.varNewVarNameField.clear();
			myState.varNewVarValue = ScriptGraphDataObject();
		}
		ImGui::EndDisabled();

		const float x = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 100);
		if (ImGui::Button("Close", ImVec2(100, 0)))
		{
			ImGui::CloseCurrentPopup();
			UpdateVariableContextMenu();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	if(!myState.varToDelete.empty())
	{
		mySchema->RemoveVariable(myState.varToDelete);
		myState.varToDelete.clear();
	}
}

void ScriptGraphEditor::HandleEditorCreate()
{
	if (ImNodeEd::BeginCreate())
	{
		ImNodeEd::PinId startLinkId, endLinkId;

		// This returns True constantly while trying to create a link, even before we've selected an end pin.
		if (ImNodeEd::QueryNewLink(&startLinkId, &endLinkId))
		{
			if (startLinkId && endLinkId)
			{

				std::string canCreateEdgeFeedback;
				if(!mySchema->CanCreateEdge(startLinkId.Get(), endLinkId.Get(), canCreateEdgeFeedback))
				{
					ImNodeEd::RejectNewItem(ImColor(255, 0, 0, 255));
				}

				// This is true if we've made a link between two pins. I.e. when we release LMB to make a link.
				if (ImNodeEd::AcceptNewItem())
				{
					mySchema->CreateEdge(startLinkId.Get(), endLinkId.Get());
				}
			}
		}
	}
	ImNodeEd::EndCreate();
}

void ScriptGraphEditor::HandleEditorDelete()
{
	if(ImNodeEd::BeginDelete())
	{
		ImNodeEd::LinkId deletedLinkId;
		while(ImNodeEd::QueryDeletedLink(&deletedLinkId))
		{
			if(ImNodeEd::AcceptDeletedItem())
			{
				mySchema->RemoveEdge(deletedLinkId.Get());
			}
		}

		ImNodeEd::NodeId deletedNodeId = 0;
		while(ImNodeEd::QueryDeletedNode(&deletedNodeId))
		{
			if(ImNodeEd::AcceptDeletedItem())
			{
				mySchema->RemoveNode(deletedNodeId.Get());
			}
		}
	}
	ImNodeEd::EndDelete();
}

void ScriptGraphEditor::RenderNode(const ScriptGraphNode* aNode)
{
	// Data collection
	std::vector<const ScriptGraphPin*> inputPins;
	std::vector<const ScriptGraphPin*> outputPins;

	ImVec2 headerTextSize = { 0, 0 };
	if(aNode->IsExecNode() || !aNode->IsSimpleNode())
	{
		headerTextSize = ImGui::CalcTextSize(aNode->GetNodeTitle().c_str());
	}

	const GraphColor nodeHeaderColor = aNode->GetNodeHeaderColor();

	// Figure out minimum size of the future table columns.
	ImVec2 leftMinSize = { 64, 0 };
	ImVec2 rightMinSize = { 64, 0 };
	if(!aNode->IsExecNode() && aNode->IsSimpleNode())
	{
		leftMinSize.x /= 2.0f;
		rightMinSize.x /= 2.0f;
	}

	for (auto& [uid, pin] : aNode->GetPins())
	{
		ImVec2 currentTextSize = ImGui::CalcTextSize(pin.GetLabel().c_str());
		if(pin.GetPinDirection() == PinDirection::Input && ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(pin.GetDataType()))
		{
			// TODO: This is a hack to accommodate the edit widget.
			// Needs to be a more intelligent way of measuring the size.
			currentTextSize.x += 50;
		}

		if (pin.GetPinDirection() == PinDirection::Input)
		{
			if (currentTextSize.x > leftMinSize.x)
				leftMinSize.x = currentTextSize.x;
			if (currentTextSize.y > leftMinSize.y)
				leftMinSize.y = currentTextSize.y;

			inputPins.push_back(&pin);
		}
		else
		{
			if (currentTextSize.x > rightMinSize.x)
				rightMinSize.x = currentTextSize.x;
			if (currentTextSize.y > rightMinSize.y)
				rightMinSize.y = currentTextSize.y;

			outputPins.push_back(&pin);
		}
	}

	std::sort(inputPins.begin(), inputPins.end(), [](const ScriptGraphPin* A, const ScriptGraphPin* B) { return A->GetUID() < B->GetUID(); });
	std::sort(outputPins.begin(), outputPins.end(), [](const ScriptGraphPin* A, const ScriptGraphPin* B) { return A->GetUID() < B->GetUID(); });

	if (leftMinSize.x > 0)
		leftMinSize.x += 48;

	if (rightMinSize.x > 0)
		rightMinSize.x += 48;

	// Prep node style vars.
	const bool isGetVarNode = dynamic_cast<const SGNode_GetVariable*>(aNode);
	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeRounding, isGetVarNode ? 30.0f : 3.0f);
	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

	// Begin the node.
	const auto uidAwareBase = AsGUIDAwarePtr(aNode);
	const ImNodeEd::NodeId currentNodeId = uidAwareBase->GetUID();
	ImNodeEd::BeginNode(currentNodeId);
	ImGui::PushID(static_cast<int>(uidAwareBase->GetUID()));

	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos({ cursorPos.x, cursorPos.y + 2 });

	const float bodyMinWidth = leftMinSize.x + rightMinSize.x + 16;
	const ImVec2 nodePinTableSize = { headerTextSize.x > bodyMinWidth ? headerTextSize.x : bodyMinWidth, 0 };

	if(aNode->IsExecNode() || !aNode->IsSimpleNode())
	{
		ImGui::BeginTable("nodeHeader", 2, ImGuiTableFlags_SizingFixedFit, nodePinTableSize);
		ImGui::TableNextColumn();
		if(const auto It = myNodeTypeIcons.find(aNode->GetNodeType()); It != myNodeTypeIcons.end())
		{
			const ImTextureID funcTextureId = (void*)It->second->GetShaderResourceView();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f);
			ImGui::Image(funcTextureId, { 16, 16 }, { 0, 0 }, { 1, 1 }, { 255, 255, 255, 255 });
		}
		ImGui::TableNextColumn();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2.0f);
		ImGui::TextUnformatted(aNode->GetNodeTitle().c_str());
		ImGui::EndTable();
	}
	
	ImVec2 headerRectMin = ImGui::GetItemRectMin();
	headerRectMin.y -= 2;

	// Node Pins
	ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp, nodePinTableSize);
	ImGui::TableNextRow();
	const size_t numRows = inputPins.size() > outputPins.size() ? inputPins.size() : outputPins.size();
	for (size_t row = 0; row < numRows; row++)
	{
		ImGui::TableNextColumn();
		if (row < inputPins.size())
		{
			const ScriptGraphPin* inputPin = inputPins[row];
			RenderPin(inputPin);
		}
		ImGui::TableNextColumn();
		if (row < outputPins.size())
		{
			const ScriptGraphPin* outputPin = outputPins[row];
			RenderPin(outputPin);
		}
		//ImGui::TableNextRow();
	}
	ImGui::EndTable();
	const ImVec2 nodeBodyMin = ImGui::GetItemRectMin();
	const ImVec2 nodeBodyMax = ImGui::GetItemRectMax();	
	ImNodeEd::EndNode();

	if (aNode->IsExecNode() || !aNode->IsSimpleNode())
	{
		const float halfBorderWidth = ImNodeEd::GetStyle().NodeBorderWidth * 0.5f;
		const ImRect nodeHeader = ImRect(headerRectMin.x - (8 - halfBorderWidth), headerRectMin.y - (4 - halfBorderWidth), nodeBodyMax.x + (8 - halfBorderWidth), nodeBodyMin.y - 2);

		ImDrawList* nodeDrawList = ImNodeEd::GetNodeBackgroundDrawList(uidAwareBase->GetUID());

		const ImTextureID nodeTextureId = (void*)myNodeHeaderTexture->GetShaderResourceView();
		nodeDrawList->AddImageRounded(nodeTextureId,
			nodeHeader.Min,
			nodeHeader.Max,
			ImVec2(0.1f, 0.2f), ImVec2(0.8f, 0.8f),
			nodeHeaderColor.AsU32(),
			ImNodeEd::GetStyle().NodeRounding,
			1 | 2
			);

		const ImRect headerSeparatorRect = ImRect(nodeHeader.GetBL(), nodeHeader.GetBR());

		nodeDrawList->AddLine(
			ImVec2(headerSeparatorRect.GetTL().x, headerSeparatorRect.GetTL().y + -0.5f),
			ImVec2(headerSeparatorRect.GetTR().x - 1, headerSeparatorRect.GetTR().y + -0.5f),
			ImColor(64, 64, 64, 255), 1.0f);
	}

	if (isGetVarNode)
	{
		const float halfBorderWidth = ImNodeEd::GetStyle().NodeBorderWidth * 0.5f;
		const ImRect nodeHeader = ImRect(nodeBodyMin.x, nodeBodyMin.y, nodeBodyMax.x, nodeBodyMax.y);

		ImDrawList* nodeDrawList = ImNodeEd::GetNodeBackgroundDrawList(uidAwareBase->GetUID());

		const ImTextureID nodeTextureId = (void*)myGetterGradient->GetShaderResourceView();
		nodeDrawList->AddImageRounded(nodeTextureId,
			nodeHeader.Min,
			nodeHeader.Max,
			ImVec2(0.1f, 0.2f), ImVec2(0.8f, 0.8f),
			nodeHeaderColor.AsU32(),
			ImNodeEd::GetStyle().NodeRounding,
			1 | 2
		);
	}

	ImGui::PopID();

	ImNodeEd::PopStyleVar();
	ImNodeEd::PopStyleVar();
	
}

void ScriptGraphEditor::RenderPin(const ScriptGraphPin* aPin)
{
	if (aPin->GetPinDirection() == PinDirection::Input)
	{
		const bool CanConstructInPlace = ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(aPin->GetDataType());

		ImNodeEd::BeginPin(aPin->GetUID(), static_cast<ImNodeEd::PinKind>(aPin->GetPinDirection()));
		const GraphColor nodeHeaderColor = ScriptGraphDataTypeRegistry::GetDataTypeColor(aPin->GetDataType());
		DrawPinIcon(aPin, { nodeHeaderColor.R, nodeHeaderColor.G, nodeHeaderColor.B, nodeHeaderColor.A }, aPin->IsPinConnected());
		ImNodeEd::EndPin();
		ImGui::SameLine();

		if (aPin->IsLabelVisible())
		{
			const float y = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(y + 3);
			ImGui::TextUnformatted(aPin->GetLabel().c_str());
			ImGui::SetCursorPosY(y);
		}

		if (!aPin->IsPinConnected() && CanConstructInPlace)
		{
			ImGui::SameLine();
			const float y = ImGui::GetCursorPosY();

			ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(aPin->GetDataType(), aPin->GetUUID(), aPin->GetPtr());

			// Hax to force ImGui to move the cursor back to where it should be.
			ImGui::SetCursorPosY(y + 3);
			ImGui::Dummy({ 0, 0 });
		}
	}
	else
	{
		if (aPin->IsLabelVisible())
		{
			ImGui::SetCursorPosX(
				ImGui::GetCursorPosX()
				+ ImGui::GetColumnWidth()
				- ImGui::CalcTextSize(aPin->GetLabel().c_str()).x
				- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x
				- 14.0f
			);

			const float y = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(y + 3);
			ImGui::TextUnformatted(aPin->GetLabel().c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosY(y);
		}
		else
		{
			ImGui::SetCursorPosX(
				ImGui::GetCursorPosX()
				+ ImGui::GetColumnWidth()
				- ImGui::GetScrollX() - ImGui::GetStyle().ItemSpacing.x
				- 14.0f
			);
		}

		ImNodeEd::BeginPin(aPin->GetUID(), static_cast<ImNodeEd::PinKind>(aPin->GetPinDirection()));
		const GraphColor nodeHeaderColor = ScriptGraphDataTypeRegistry::GetDataTypeColor(aPin->GetDataType());
		DrawPinIcon(aPin, { nodeHeaderColor.R, nodeHeaderColor.G, nodeHeaderColor.B, nodeHeaderColor.A }, aPin->IsPinConnected());
		ImNodeEd::EndPin();
	}
}

void ScriptGraphEditor::DrawPinIcon(const ScriptGraphPin* aPin, ImVec4 aPinColor, bool isConnected) const
{
	const PinIcon icon = aPin->GetType() == ScriptGraphPinType::Exec ? PinIcon::Exec : PinIcon::Circle;

	const float iconSize = 24.0f;
	const ImVec2 sizeRect = ImVec2(iconSize, iconSize);
	if (ImGui::IsRectVisible(sizeRect))
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImRect drawRect = { cursorPos, { cursorPos.x + iconSize, cursorPos.y + iconSize } };
		const float outlineScale = drawRect.GetWidth() / (iconSize * 2.0f);
		const int circleExtraSegments = static_cast<int>(round(2 * outlineScale));

		unsigned color = IM_COL32(aPinColor.x, aPinColor.y, aPinColor.z, aPinColor.w);

		const ImColor innerColor(32, 32, 32, 255);

		switch (icon)
		{
		case PinIcon::Exec:
		{
			const auto origin_scale = drawRect.GetWidth() / iconSize;

			const auto offset_x = 1.0f * origin_scale;
			const auto offset_y = 0.0f * origin_scale;
			const auto margin = (isConnected ? 2.0f : 2.0f) * origin_scale;
			const auto rounding = 0.1f * origin_scale;
			const auto tip_round = 0.7f;

			const float canvasX = drawRect.GetTL().x + margin + offset_x;
			const float canvasY = drawRect.GetTL().y + margin + offset_y;
			const float canvasW = canvasX + drawRect.GetWidth() - margin * 2.0f;
			const float canvasH = canvasY + drawRect.GetWidth() - margin * 2.0f;

			const auto canvas = ImRect(canvasX, canvasY, canvasW, canvasH);

			const auto left = canvas.GetTL().x + canvas.GetWidth() * 0.5f * 0.3f;
			const auto right = canvas.GetTL().x + canvas.GetWidth() - canvas.GetWidth() * 0.5f * 0.3f;
			const auto top = canvas.GetTL().y + canvas.GetHeight() * 0.5f * 0.2f;
			const auto bottom = canvas.GetTL().y + canvas.GetHeight() - canvas.GetHeight() * 0.5f * 0.2f;
			const auto center_y = (top + bottom) * 0.5f;

			const auto tip_top = ImVec2(canvas.GetTL().x + canvas.GetWidth() * 0.5f, top);
			const auto tip_right = ImVec2(right, center_y);
			const auto tip_bottom = ImVec2(canvas.GetTL().x + canvas.GetWidth() * 0.5f, bottom);
			
			drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
			drawList->PathBezierCurveTo(
				ImVec2(left, top),
				ImVec2(left, top),
				ImVec2(left, top) + ImVec2(rounding, 0));
			drawList->PathLineTo(tip_top);
			drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
			drawList->PathBezierCurveTo(
				tip_right,
				tip_right,
				tip_bottom + (tip_right - tip_bottom) * tip_round);
			drawList->PathLineTo(tip_bottom);
			drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
			drawList->PathBezierCurveTo(
				ImVec2(left, bottom),
				ImVec2(left, bottom),
				ImVec2(left, bottom) - ImVec2(0, rounding));

			if (!isConnected)
			{
				if (innerColor & 0xFF000000)
					drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

				drawList->PathStroke(color, true, 2.0f * outlineScale);
			}
			else
				drawList->PathFillConvex(color);
		}
		break;
		case PinIcon::Circle:
		{
			const float triangleStart = (drawRect.GetTL().x + iconSize / 2) * 0.32f * drawRect.GetWidth();
			drawRect.Min.x -= static_cast<int>(round(drawRect.GetWidth() * 0.25f * 0.25f));
			const ImVec2 center = { drawRect.GetTL().x + sizeRect.x / 2, drawRect.GetTL().y + sizeRect.y / 2 };

			if (!isConnected)
			{
				const auto r = 0.5f * drawRect.GetWidth() / 2.0f - 0.5f;

				if (innerColor & 0xFF000000)
					drawList->AddCircleFilled(center, r, innerColor, 12 + circleExtraSegments);
				drawList->AddCircle(center, r, color, 12 + circleExtraSegments, 2.0f * outlineScale);
			}
			else
				drawList->AddCircleFilled(center, 0.5f * drawRect.GetWidth() / 2.0f, color, 12 + circleExtraSegments);
		}
		break;
		}
	}

	ImGui::Dummy(sizeRect);
}

void ScriptGraphEditor::HandleScriptGraphError(const ScriptGraph& aGraph,
	size_t aNodeUID, const std::string& anErrorMessage)
{
	// Don't report multiple errors, just the first one.
	if(!myState.errorIsErrorState)
	{
		myState.errorIsErrorState = true;
		myState.errorMessage = anErrorMessage;
		myState.errorNodeId = aNodeUID;

		ImNodeEd::SelectNode(myState.errorNodeId);
		ImNodeEd::NavigateToSelection();
		ImNodeEd::DeselectNode(myState.errorNodeId);

		myState.isTicking = false;
	}
}

void ScriptGraphEditor::ClearErrorState()
{
	myState.errorIsErrorState = false;
	myState.errorMessage = "";
	myState.errorNodeId = 0;
}

void ScriptGraphEditor::LoadTextureFromMemory(Tga::Texture* outTexture, const std::wstring& aName, const unsigned char* someImageData, size_t anImageDataSize, const D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
{
	ID3D11ShaderResourceView* srv;
	ID3D11Resource* res;
	HRESULT result = DirectX::CreateDDSTextureFromMemory(
		Tga::DX11::Device,
		someImageData,
		anImageDataSize,
		&res,
		&srv
	);

	outTexture->SetShaderResourceView(srv);
}

void ScriptGraphEditor::DuplicateSelectedNodes()
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);

	// Get selected nodes	
	std::vector<ImNodeEd::NodeId> selectedNodes;
	selectedNodes.resize(ImNodeEd::GetSelectedObjectCount());
	int nodeCount = ImNodeEd::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
	selectedNodes.resize(nodeCount); // This was part of the official demo so im keeping it just in case

	ImNodeEd::ClearSelection();

	// Handle nodes
	std::vector<std::shared_ptr<ScriptGraphNode>> newNodes;
	std::unordered_map<size_t, size_t> newPinIDs;
	newNodes.reserve(nodeCount);
	for (auto& [nodeUID, node] : myGraph->GetNodes())
	{
		ImNodeEd::NodeId nodeId = nodeUID;
		if (std::find(selectedNodes.begin(), selectedNodes.end(), nodeId) == selectedNodes.end() || node->IsEntryNode())
		{
			continue;
		}

		// Create new node
		auto newNode = mySchema->AddNode(node->GetClass().TypeName);
		const auto uidAwareBase = AsGUIDAwareSharedPtr(newNode);
		const size_t newNodeUID = uidAwareBase->GetUID();
		const ImNodeEd::NodeId newNodeId = newNodeUID;

		// Offset new nodes slightly from original
		float x = 0.f, y = 0.f, z = 0.f;
		node->GetNodePositionCache(x, y, z);
		x += 10.f;
		y += 10.f;
		newNode->UpdateNodePositionCache(x, y, z);
		ImNodeEd::SetNodePosition(newNodeUID, { x, y });
		ImNodeEd::SetNodeZPosition(newNodeUID, z);

		// Copy pin data
		int index = 0;
		for (auto& [pinId, pin] : node->GetPins())
		{
			if (pin.IsPinConnected())
			{
				// Used to connect the new edges
				newPinIDs.emplace(pinId, newNode->GetPin(pin.GetLabel()).GetUID());
				continue;
			}
			if (pin.GetType() != ScriptGraphPinType::Data)
			{
				continue;
			}

			auto& newPin = const_cast<ScriptGraphPin&>(newNode->GetPin(pin.GetLabel()));
			if (pin.GetDataType() == typeid(std::string))
			{
				std::string string;
				pin.GetData(string);
				newPin.SetData(string);
			}
			else
			{
				size_t dataSize = pin.GetDataSize();
				void* dataPtr = new char[dataSize];
				if (pin.GetRawData(dataPtr, dataSize))
				{
					newPin.SetRawData(dataPtr, dataSize);
				}
				delete[] dataPtr;
			}
			++index;
		}

		// Select the new node
		ImNodeEd::SelectNode(newNodeId, true);
	}

	// Copy links between copied nodes
	for (auto& [edgeUID, edge] : myGraph->GetEdges())
	{
		if (newPinIDs.find(edge.FromUID) == newPinIDs.end() ||
			newPinIDs.find(edge.ToUID) == newPinIDs.end())
		{
			continue;
		}
		mySchema->CreateEdge(newPinIDs.find(edge.FromUID)->second, newPinIDs.find(edge.ToUID)->second);
	}
	ImNodeEd::SetCurrentEditor(nullptr);
}

void ScriptGraphEditor::SaveSelectedNodes()
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);

	// Get selected nodes
	std::vector<ImNodeEd::NodeId> selectedNodes;
	selectedNodes.resize(ImNodeEd::GetSelectedObjectCount());
	int nodeCount = ImNodeEd::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
	selectedNodes.resize(nodeCount); // This was part of the official demo so im keeping it just in case

	// Save the selected nodes
	mySelectedNodesSave.clear();
	for (const auto& nodeId : selectedNodes)
	{
		mySelectedNodesSave.push_back(nodeId);
	}

	ImNodeEd::ClearSelection();
	ImNodeEd::SetCurrentEditor(nullptr);
}

void ScriptGraphEditor::PasteDuplicateNodes()
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);

	// Get the current mouse position
	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos = ImNodeEd::ScreenToCanvas(mousePos);

	// Find the original positions of saved nodes to calculate offset
	ImVec2 originalCenter{ 0.0f, 0.0f };
	int nodeCount = 0;
	for (auto& nodeId : mySelectedNodesSave)
	{
		if (auto nodeIter = myGraph->GetNodes().find(nodeId.Get()); nodeIter != myGraph->GetNodes().end())
		{
			float x = 0.f, y = 0.f, z = 0.f;
			nodeIter->second->GetNodePositionCache(x, y, z);
			originalCenter.x += x;
			originalCenter.y += y;
			nodeCount++;
		}
	}
	if (nodeCount > 0)
	{
		originalCenter.x /= nodeCount;
		originalCenter.y /= nodeCount;
	}

	// Store mapping between old and new node IDs
	std::unordered_map<size_t, size_t> oldToNewNodeMap;
	std::vector<std::shared_ptr<ScriptGraphNode>> newNodes;
	std::unordered_map<size_t, size_t> newPinIDs;

	// First pass: Create all nodes
	for (auto& nodeId : mySelectedNodesSave)
	{
		auto nodeIter = myGraph->GetNodes().find(nodeId.Get());
		if (nodeIter == myGraph->GetNodes().end() || nodeIter->second->IsEntryNode())
		{
			continue;
		}

		auto& node = nodeIter->second;

		// Create new node
		auto newNode = mySchema->AddNode(node->GetClass().TypeName);
		const auto uidAwareBase = AsGUIDAwareSharedPtr(newNode);
		const size_t newNodeUID = uidAwareBase->GetUID();

		// Store mapping
		oldToNewNodeMap[nodeId.Get()] = newNodeUID;
		newNodes.push_back(newNode);

		// Calculate position offset from center
		float x = 0.f, y = 0.f, z = 0.f;
		node->GetNodePositionCache(x, y, z);
		float offsetX = x - originalCenter.x;
		float offsetY = y - originalCenter.y;

		// Position relative to mouse
		x = mousePos.x + offsetX;
		y = mousePos.y + offsetY;

		// Update new node position
		newNode->UpdateNodePositionCache(x, y, z);
		ImNodeEd::SetNodePosition(newNodeUID, { x, y });
		ImNodeEd::SetNodeZPosition(newNodeUID, z);

		// Copy pin data
		for (auto& [pinId, pin] : node->GetPins())
		{
			auto& newPin = const_cast<ScriptGraphPin&>(newNode->GetPin(pin.GetLabel()));
			newPinIDs[pinId] = newPin.GetUID();

			if (pin.GetType() == ScriptGraphPinType::Data)
			{
				if (pin.GetDataType() == typeid(std::string))
				{
					std::string string;
					pin.GetData(string);
					newPin.SetData(string);
				}
				else
				{
					size_t dataSize = pin.GetDataSize();
					void* dataPtr = new char[dataSize];
					if (pin.GetRawData(dataPtr, dataSize))
					{
						newPin.SetRawData(dataPtr, dataSize);
					}
					delete[] dataPtr;
				}
			}
		}

		// Select the new node
		ImNodeEd::SelectNode(newNodeUID, true);
	}

	// Second pass: Recreate connections between copied nodes
	for (auto& nodeId : mySelectedNodesSave)
	{
		auto nodeIter = myGraph->GetNodes().find(nodeId.Get());
		if (nodeIter == myGraph->GetNodes().end())
		{
			continue;
		}

		auto& node = nodeIter->second;
	}

	ImNodeEd::SetCurrentEditor(nullptr);
}

void ScriptGraphEditor::Init()
{
	// TODO: Comment this out if you don't want to use the node header image.
	// Otherwise change it to be your own texture loader from memory.
	myNodeHeaderTexture = new Tga::Texture();
	LoadTextureFromMemory(myNodeHeaderTexture, L"NodeGradientHeader", BuiltIn_ScriptGraphNodeGradient_ByteCode, sizeof(BuiltIn_ScriptGraphNodeGradient_ByteCode), nullptr);

	Tga::Texture* eventIconBase = new Tga::Texture();
	LoadTextureFromMemory(eventIconBase, L"NodeEventIcon", BuiltIn_Event_Icon_ByteCode, sizeof(BuiltIn_Event_Icon_ByteCode), nullptr);
	myNodeTypeIcons.emplace(ScriptGraphNodeType::Event, std::move(eventIconBase));

	Tga::Texture* nodeFunctionIcon = new Tga::Texture();
	LoadTextureFromMemory(nodeFunctionIcon, L"NodeFunctionIcon", BuiltIn_Function_Icon_ByteCode, sizeof(BuiltIn_Function_Icon_ByteCode), nullptr);
	myNodeTypeIcons.emplace(ScriptGraphNodeType::Undefined, std::move(nodeFunctionIcon));

	myGetterGradient = new Tga::Texture();
	LoadTextureFromMemory(myGetterGradient, L"NodeGetterGradient", BuiltIn_GetGradient_ByteCode, sizeof(BuiltIn_GetGradient_ByteCode), nullptr);

	nodeEditorContext = ImNodeEd::CreateEditor(&nodeEditorCfg);

	if(myBgContextCategories.empty())
	{
		const auto supportedNodeTypes = ScriptGraphSchema::GetSupportedNodeTypes();
		for(const auto& [nodeName, nodeType] : supportedNodeTypes)
		{
			if (nodeType.DefaultObject->IsInternalOnly())
				continue;

			const std::string nodeCategory = nodeType.DefaultObject->GetNodeCategory();
			const auto uuidBase = AsGUIDAwareSharedPtr(nodeType.DefaultObject);
			if(auto catIt = myBgContextCategories.find(nodeCategory); catIt != myBgContextCategories.end())
			{
				catIt->second.Items.push_back({ nodeType.DefaultObject->GetNodeTitle(), uuidBase->GetTypeName(), ""});
			}
			else
			{				
				myBgContextCategories.insert({ nodeCategory, {nodeCategory, {{ nodeType.DefaultObject->GetNodeTitle(), uuidBase->GetTypeName() }} }});
				myBgContextCategoryNamesList.push_back(nodeCategory);
			}
		}	

		std::sort(myBgContextCategoryNamesList.begin(), myBgContextCategoryNamesList.end());
	}

	myState.bgCtxtSearchFieldResults.reserve(ScriptGraphSchema::GetSupportedNodeTypes().size());
	myState.initNavToContent = true;

	//~ Begin Temp. This should be replaced with initing when loading a graph.
	myGraph = ScriptGraphSchema::CreateScriptGraph();
	myGraph->BindErrorHandler([this](const ScriptGraph& aGraph, size_t aNodeUID, const std::string& aMessage)
		{
			HandleScriptGraphError(aGraph, aNodeUID, aMessage);
		});

	mySchema = myGraph->GetGraphSchema();


	mySchema->AddNode<SGNode_MathAdd>();
	mySchema->AddNode<SGNode_MathAdd>();


	//~ End temp

	UpdateVariableContextMenu();
}

void ScriptGraphEditor::Update(float aDeltaTime)
{
	if(myState.isTicking)
	{
		myState.flowShowFlow = true;
		myGraph->Tick(aDeltaTime);
		
	}
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)&&ImGui::IsKeyReleased(ImGuiKey_C))
	{
		SaveSelectedNodes();
	}
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyReleased(ImGuiKey_V))
	{
		PasteDuplicateNodes();
	}


	if (GameObjectRegistery::Get().HasGameObjectCollided())
	{
		myGraph->OnTriggerEntry(GameObjectRegistery::Get().GetCollidedGameObjects()[0], GameObjectRegistery::Get().GetCollidedGameObjects()[1]);
	}
	else
	{
		myGraph->OnTriggerExit(GameObjectRegistery::Get().GetPreviousGameObjects()[0], GameObjectRegistery::Get().GetPreviousGameObjects()[1]);
	}
}

void ScriptGraphEditor::Render()
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	// BUG: Breaks if the window is too small, like with brand new window is not present in imgui.ini!
	// Fixed this myself by editing ImNodeEd::Begin() to return false if canvas clipping fails.
	// Have posted on his GitHub about that @ https://github.com/thedmd/imgui-node-editor/issues/191
	ImGui::SetNextWindowSize({ 1280, 720}, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("ScriptGraph Editor"))
	{
		ImGui::SetNextItemWidth(50);
		if(ImGui::Button(">"))
		{
			ImGui::OpenPopup("TriggerEntryPoint");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Open the Trigger Entry Point dialog.");
		}
		ImGui::SameLine();
		if(ImGui::Checkbox("Should Tick", &myState.isTicking))
		{
			if(myState.isTicking)
				ClearErrorState();
			myGraph->SetTicking(myState.isTicking);
		}

		ImGui::SameLine();
		if(ImGui::Button("Variables"))
		{
			ImGui::OpenPopup("Edit Variables");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Open the Edit Variables dialog.");
		}

		ImGui::SameLine();
		if(ImGui::Button("Save"))
		{
			std::string outGraph;

			ScriptGraphSchema::SerializeScriptGraph(myGraph, outGraph);
			std::ofstream file("file.txt");
			file << outGraph;
			file.close();
		}
		ImGui::SameLine();
		if(ImGui::Button("Load"))
		{
			std::ifstream file("file.txt");
			const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			file.close();

			if(!inGraph.empty())
			{
				myGraph->UnbindErrorHandler();
				myGraph = nullptr;

				ScriptGraphSchema::DeserializeScriptGraph(myGraph, inGraph);
				// Need to loop through and set the ImNodeEd pins xP
				for (auto& [nodeUID, node] : myGraph->GetNodes())
				{
					float x, y, z;
					mySchema->GetNodePositionCache(node, x, y, z);
					ImNodeEd::SetNodePosition(nodeUID, { x, y });
					ImNodeEd::SetNodeZPosition(nodeUID, z);
				}

				myGraph->BindErrorHandler([this](const ScriptGraph& aGraph, size_t aNodeUID, const std::string& aMessage)
					{
						HandleScriptGraphError(aGraph, aNodeUID, aMessage);
					});

				mySchema = nullptr;
				mySchema = myGraph->GetGraphSchema();
				UpdateVariableContextMenu();
				myState.initNavToContent = true;
			}
		}
		
		if(ImNodeEd::Begin("A Node Editor"))
		{
			for (const auto& [nodeUID, node] : myGraph->GetNodes())
			{
				if (myState.errorIsErrorState && nodeUID == myState.errorNodeId)
				{
					ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeBorderWidth, 10.0f);
					ImNodeEd::PushStyleColor(ImNodeEd::StyleColor_NodeBorder, ImVec4(0.75f, 0, 0, 1));
				}

				RenderNode(node.get());

				if (myState.errorIsErrorState && nodeUID == myState.errorNodeId)
				{
					ImNodeEd::PopStyleVar();
					ImNodeEd::PopStyleColor();

					if(ImNodeEd::GetHoveredNode().Get() == nodeUID)
					{
						ImNodeEd::Suspend();
						ImGui::SetTooltip(myState.errorMessage.c_str());
						ImNodeEd::Resume();
					}
				}

				// Cache node positions, sigh.
				// NOTE: This just happens because we can't read node positions
				// from ImNodeEd if we don't have an active ImGui window with
				// that node editor, buh.
				const ImVec2 nodePos = ImNodeEd::GetNodePosition(nodeUID);
				const float nodeZ = ImNodeEd::GetNodeZPosition(nodeUID);
				mySchema->UpdateNodePositionCache(node, nodePos.x, nodePos.y, nodeZ);
			}

			for (const auto& [edgeId, edge] : myGraph->GetEdges())
			{
				const GraphColor typeColor = edge.Color.AsNormalized();
				ImNodeEd::Link(edge.EdgeId, edge.FromUID, edge.ToUID, { typeColor.R, typeColor.G, typeColor.B, typeColor.A }, edge.Thickness);
			}

			if(myState.flowShowFlow || !myGraph->GetLastExecutedPath().empty())
			{
				for (const auto& edgeId : myGraph->GetLastExecutedPath())
				{
					ImNodeEd::Flow(edgeId, ax::NodeEditor::FlowDirection::Forward);
				}
				myState.flowShowFlow = false;
				//myState.lastFlowCount = myGraph->GetLastExecutedPath().size();
				myGraph->ResetLastExecutedPath();
			}

			HandleEditorCreate();
			HandleEditorDelete();

			ImNodeEd::Suspend();

			if (ImNodeEd::ShowBackgroundContextMenu())
			{
				myState.bgCtxtSearchFocus = true;
				ImGui::OpenPopup("BackgroundContextMenu");
			}
			else if (ImNodeEd::NodeId nodeId; ImNodeEd::ShowNodeContextMenu(&nodeId))
			{
				ImGui::OpenPopup("NodeContextMenu");
			}

			BackgroundContextMenu();
			//NodeContextMenu();

			ImNodeEd::Resume();

			if(myState.initNavToContent)
			{
				ImNodeEd::NavigateToContent();
				myState.initNavToContent = false;
			}

			ImNodeEd::End();
		}		

		TriggerEntryPoint();
		EditVariables();
	}

	ImGui::End();
	ImNodeEd::SetCurrentEditor(nullptr);
}
