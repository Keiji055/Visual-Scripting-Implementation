#include "AABB2D.h"
#include <tge/drawers/LineDrawer.h>
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>
AABB2D::AABB2D()
{
}

AABB2D::~AABB2D()
{
}

void AABB2D::InitAABB(const Tga::Vector2f& aPosition, const float aSize)
{
	mySize = { aSize, aSize };
	SetPosition(aPosition);
	ChangeColor(1, 1, 0);
}

const Tga::Vector2f AABB2D::GetMin() const
{
	return { (myPosition.x - (mySize.x * 0.5f)), (myPosition.y - (mySize.y * 0.5f)) };
}

const Tga::Vector2f AABB2D::GetMax() const
{
	return { (myPosition.x + (mySize.x * 0.5f)) , (myPosition.y + (mySize.y * 0.5f)) };
}

const Tga::Vector2f& AABB2D::GetPosition() const
{
	return myPosition;
}

const Tga::Vector2f& AABB2D::GetSize() const
{
	return mySize;
}

bool AABB2D::IsInside(const Tga::Vector2f& aPosition) const
{
	return aPosition.x >= GetMin().x && aPosition.x <= GetMax().x &&
		aPosition.y >= GetMin().y && aPosition.y <= GetMax().y;
}

void AABB2D::SetPosition(const Tga::Vector2f& aPosition)
{
	myPosition = aPosition;
	UpdateDebugLinePosition();
}

bool AABB2D::Intersects(const AABB2D& aRange)
{
	const float halfRangeSize = aRange.mySize.x * 0.5f;
	const float myHalfSize = mySize.x * 0.5f;
	return !(aRange.myPosition.x - halfRangeSize > myPosition.x + myHalfSize ||
		aRange.myPosition.x + halfRangeSize < myPosition.x - myHalfSize ||
		aRange.myPosition.y - halfRangeSize > myPosition.y + myHalfSize ||
		aRange.myPosition.y + halfRangeSize < myPosition.y - myHalfSize);
}

void AABB2D::DebugDraw()
{
	Tga::LineDrawer& dbg = Tga::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	for (int i = 0; i < 4; i++)
	{
		dbg.Draw(Sides[i]);
	}
}

void AABB2D::ChangeColor(const float R, const float G, const float B)
{
	for (size_t i = 0; i < Sides.size(); i++)
	{
		Sides[i].color = { R,G,B,1.0f };
	}
}

void AABB2D::UpdateDebugLinePosition()
{
	Sides[0].fromPosition = { GetMin().x, GetMax().y, 0.0f };
	Sides[0].toPosition = { GetMax().x, GetMax().y, 0.0f };

	Sides[1].fromPosition = { GetMin().x, GetMax().y, 0.0f };
	Sides[1].toPosition = { GetMin().x, GetMin().y, 0.0f };

	Sides[2].fromPosition = { GetMin().x, GetMin().y, 0.0f };
	Sides[2].toPosition = { GetMax().x, GetMin().y, 0.0f };

	Sides[3].fromPosition = { GetMax().x, GetMin().y, 0.0f };
	Sides[3].toPosition = { GetMax().x, GetMax().y, 0.0f };
}