#include "GameObject.h"
#include "tge/engine.h"
#include "tge/settings/settings.h"
#include "tge/texture/TextureManager.h"
#include <string>

GameObject::GameObject()
{
	myID = 0;
	myPos = { 0,0,0 };
}

GameObject::~GameObject()
{
}

void GameObject::Init(std::string aFileName)
{
	if (Tga::Engine::GetInstance()->GetTextureManager().GetTexture(Tga::Settings::ResolveEngineAssetPathW("Sprites/"+aFileName+".png").c_str()))
	{
		mySharedData.myTexture= Tga::Engine::GetInstance()->GetTextureManager().GetTexture(Tga::Settings::ResolveEngineAssetPathW("Sprites/" + aFileName + ".png").c_str());
	}
	else
	{
		std::cout << "Inputed sprite does not exist, defualt is used" << std::endl;
		mySharedData.myTexture = Tga::Engine::GetInstance()->GetTextureManager().GetTexture(Tga::Settings::ResolveEngineAssetPathW("hej").c_str());
	}
	mySpriteInstance.mySize = mySharedData.myTexture->CalculateTextureSize();
	mySpriteInstance.myPivot = { 0.5,0.5 };
	myScale = mySharedData.myTexture->CalculateTextureSize();
	mySpriteInstance.myPosition.x = myPos.x;
	mySpriteInstance.myPosition.y = myPos.y;
	Tga::Engine::GetInstance()->GetWindowSize().y;
	myHitbox = std::make_shared<Hitbox>();
	myHitbox->Init({ myPos.x,myPos.y }, myScale.x);
	myHitbox->ChangeColor(1, 0, 0);
}

void GameObject::Update(float aDeltaTime)
{
	mySpriteInstance.myPosition.x = myPos.x;
	mySpriteInstance.myPosition.y = myPos.y;
	myHitbox->UpdatePos({ myPos.x,myPos.y });
}

void GameObject::SetID()
{
	myID = myGlobalGameObjectID;
	myGlobalGameObjectID++;
}

void GameObject::SetPosition(Tga::Vector3f aPosition)
{
	myPos = aPosition;
}

Tga::Vector3f GameObject::GetPosition()
{
	return myPos;
}

int GameObject::GetID()
{
	return myID;
}

void GameObject::Render(Tga::SpriteDrawer& aDrawer)
{
	aDrawer.Draw(mySharedData, mySpriteInstance);
	if (myHitbox)
	{
		myHitbox->DebugDraw();
	}
}

void GameObject::SetScale(float aScale)
{
	if (aScale>=0&& aScale<=1)
	{

		mySpriteInstance.mySize = myScale*aScale;
	}
	else if (aScale>1)
	{
		mySpriteInstance.mySize = myScale;
	}
	else
	{
		mySpriteInstance.mySize = 0;
	}
}

std::shared_ptr<Hitbox> GameObject::GetHitBox()
{
	 return myHitbox;
}
