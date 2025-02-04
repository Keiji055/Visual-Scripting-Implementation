#include "Hitbox.h"

Hitbox::Hitbox()
{
}

Hitbox::~Hitbox()
{
}

void Hitbox::Init(Tga::Vector2f aPos, float aSize)
{
	InitAABB(aPos,aSize);
}

bool Hitbox::Update(AABB2D aAABB)
{
	return Intersects(aAABB);
}

void Hitbox::UpdatePos(Tga::Vector2f aPos)
{
	SetPosition(aPos);
}

bool Hitbox::Intersect(std::shared_ptr<Hitbox> aHitbox)
{
	return AABB2D::Intersects(*aHitbox);
}
