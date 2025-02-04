#pragma once
#include "AABB2D.h"
#include <CommonUtilities/Vector.hpp>
#include <tge/math/Vector.h>
class Hitbox:public AABB2D
{
public:
	Hitbox();
	~Hitbox();
	void Init(Tga::Vector2f aPos,float aSize);
	bool Update(AABB2D aAABB);
	void UpdatePos(Tga::Vector2f aPos);
	bool Intersect(std::shared_ptr<Hitbox> aHitbox);
private:
	CommonUtilities::Vector2f myPos;
};
