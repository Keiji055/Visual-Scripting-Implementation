#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace CommonUtilities
{
	typedef Vector4<float> Vector4f;
	typedef Vector3<float> Vector3f;
	typedef Vector2<float> Vector2f;

	//static constexpr Vector3f zero = Vector3f(0, 0, 0);
	//static constexpr Vector3f one = Vector3f(1, 1, 1);
	//static constexpr Vector3f forward = Vector3f(0, 0, 1);
	//static constexpr Vector3f back = Vector3f(0, 0, -1);
	//static constexpr Vector3f right = Vector3f(1, 0, 0);
	//static constexpr Vector3f left = Vector3f(-1, 0, 0);
	//static constexpr Vector3f up = Vector3f(0, 1, 0);
	//static constexpr Vector3f down = Vector3f(0, -1, 0);
}