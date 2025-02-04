#pragma once

#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"

namespace CommonUtilities
{
	typedef Matrix3x3<float> Matrix3x3f;
	typedef Matrix3x3<double> Matrix3x3d;
	typedef Matrix3x3<int> Matrix3x3i;

	typedef Matrix3x3<float> float3x3;
	typedef Matrix3x3<double> double3x3;
	typedef Matrix3x3<int> int3x3;



	typedef Matrix4x4<float> Matrix4x4f;
	typedef Matrix4x4<double> Matrix4x4d;
	typedef Matrix4x4<int> Matrix4x4i;

	typedef Matrix4x4<float> float4x4;
	typedef Matrix4x4<double> double4x4;
	typedef Matrix4x4<int> int4x4;
}