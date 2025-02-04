
#include "GameWorld.h"
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/DebugDrawer.h>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <tge/graphics/dx11.h>

#include "ScriptGraphEditor/ScriptGraphEditor.h"
#include "GameObjectRegistery.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld() 
{}

void GameWorld::Init()  
{
	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };
	{
		mySharedData.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/tge_logo_w.dds");

		myTGELogoInstance.myPivot = { 0.5f, 0.5f };
		myTGELogoInstance.myPosition = Tga::Vector2f{ 0.5f, 0.5f }*resolution;
		myTGELogoInstance.mySize = Tga::Vector2f{ 0.75f, 0.75f }*resolution.y;
		myTGELogoInstance.myColor = Tga::Color(1, 1, 1, 1);
	}
	GameObjectRegistery::Get().InitHitBoxCommand();

	myScriptGraphEditor = new ScriptGraphEditor();
	myScriptGraphEditor->Init();
}
void GameWorld::Update(float aTimeDelta)
{
	UNREFERENCED_PARAMETER(aTimeDelta);
	myScriptGraphEditor->Update(aTimeDelta);
}

void GameWorld::Render()
{
	auto &engine = *Tga::Engine::GetInstance();
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	// Game update
	{
		spriteDrawer.Draw(mySharedData, myTGELogoInstance);
		for (size_t i = 0; i < GameObjectRegistery::Get().GetAllGameObject().size(); i++)
		{
			GameObjectRegistery::Get().GetAllGameObject()[i]->Render(spriteDrawer);
		}
	}

	// Debug draw pivot
#ifndef _RETAIL
	{
		Tga::DebugDrawer& dbg = engine.GetDebugDrawer();
		Tga::Color c1 = myTGELogoInstance.myColor;
		dbg.DrawCircle(myTGELogoInstance.myPosition, 5.f, (c1.myR + c1.myG + c1.myB) / 3 > 0.3f ? Tga::Color(0, 0, 0, 1) : Tga::Color(1, 1, 1, 1));
	}
#endif
	myScriptGraphEditor->Render();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}