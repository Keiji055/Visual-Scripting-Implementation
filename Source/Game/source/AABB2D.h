#pragma once
#include <tge/math/Vector.h>
#include <array>
#include "tge\primitives\LinePrimitive.h"
class AABB2D
{
public:
	AABB2D();
	virtual ~AABB2D();

	void InitAABB(const Tga::Vector2f& aPosition, const float aSize = 40.0f);
	bool IsInside(const Tga::Vector2f& aPosition) const;
	void DebugDraw();
	void ChangeColor(const float R, const float G, const float B);
protected:
	bool Intersects(const AABB2D& aRange);
	const Tga::Vector2f& GetPosition() const;
	const Tga::Vector2f GetMin() const;
	const Tga::Vector2f GetMax() const;
	const Tga::Vector2f& GetSize() const;
	void SetPosition(const Tga::Vector2f& aPosition);
	void UpdateDebugLinePosition();
	Tga::Vector2f myPosition = { 0,0 };
	Tga::Vector2f mySize = { 10,10 };
	std::array<Tga::LinePrimitive, 4> Sides;


	float myID;

};