#pragma once

//Includes matrix och vector
#include "Quaternion.hpp"

namespace CommonUtilities
{
	template<typename T>
	class TransformTemplate
	{
	public:
		Vector3<T> position = Vector3<T>(T(0));
		Quaternion<T> rotation = Quaternion<T>();
		Vector3<T> scale = Vector3<T>(T(1));

		Matrix4x4<T> ToMatrix() const;

		TransformTemplate<T> operator*(const TransformTemplate<T>& aTransform) const;
	};

	typedef TransformTemplate<float> Transformf;
	typedef TransformTemplate<double> Transformd;


	template<typename T>
	inline Matrix4x4<T> TransformTemplate<T>::ToMatrix() const
	{
		Matrix4x4<T> result;

		result *= Matrix4x4<T>::CreateScaleMatrix(scale);
		result *= rotation.ToMatrix44();
		result *= Matrix4x4<T>::CreatePositionMatrix(position);
		return result;
	}

	template <class T> bool operator==(const TransformTemplate<T>& aTransform0, const TransformTemplate<T>& aTransform1)
	{
		return (aTransform0.position == aTransform1.position &&
				aTransform0.rotation == aTransform1.rotation &&
				aTransform0.scale == aTransform1.scale);
	}

	template<class T>
	inline TransformTemplate<T> TransformTemplate<T>::operator*(const TransformTemplate<T>& aTransform) const
	{
		TransformTemplate<T> newTransform;
		newTransform.position = (aTransform.rotation * position) + aTransform.position;
		newTransform.rotation = rotation * aTransform.rotation;
		newTransform.scale = aTransform.scale * scale;
		return newTransform;
	}
}
