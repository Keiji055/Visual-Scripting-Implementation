#pragma once
#include <tge/sprite/sprite.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include "Hitbox.h"
#include "tge\math\Vector.h"
#include "..\..\Source\Game\source\GlobalGameObjectID.h"


class GameObject
{
public:
	GameObject();
	~GameObject();
	void Init(std::string aFileName);
	void Update(float aDeltaTime);
	void SetID();
	void SetPosition(Tga::Vector3f aPosition);
	Tga::Vector3f GetPosition();
	int GetID();
	void Render(Tga::SpriteDrawer& aDrawer);
	void SetScale(float aScaleX);
	std::shared_ptr<Hitbox> GetHitBox();
private:
	int myID;
	Tga::Vector2f myScale;
	Tga::Sprite2DInstanceData mySpriteInstance;
	Tga::SpriteSharedData mySharedData;
	Tga::Vector3f myPos;
	std::shared_ptr<Hitbox> myHitbox;

};
