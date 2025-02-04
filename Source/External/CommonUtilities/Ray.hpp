#pragma once

#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Ray
	{
	public:
		// Default constructor: there is no ray, the origin and direction are the zero vector.
		Ray() = default;

		// Copy constructor.
		Ray(const Ray<T>& aRay) = default;

		// Constructor that takes two points that define the ray, the direction aPoint - aOrigin and the origin is aOrigin.
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);

		// Init the ray with two points, the same as the constructor above.
		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);

		// Init the ray with an origin and a direction.
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		Vector3<T> GetDirection() const;
		Vector3<T> GetOrigin() const;

		void SetDirection(const Vector3<T>& aDirection);
		void SetOrigin(const Vector3<T>& aOrigin);

	private:
		Vector3<T> myOrigin;
		Vector3<T> myDirection;
	};

	typedef Ray<float> Rayf;
	typedef Ray<double> Rayd;

	template<class T>
	inline Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myOrigin = aOrigin;
		myDirection = Vector3<T>(aPoint - aOrigin).GetNormalized();
	}

	template<class T>
	inline void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myOrigin = aOrigin;
		myDirection = Vector3<T>(aPoint - aOrigin).GetNormalized();
	}

	template<class T>
	inline void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection.GetNormalized();
	}


	template<class T>
	inline Vector3<T> Ray<T>::GetDirection() const
	{
		return myDirection;
	}
	template<class T>
	inline Vector3<T> Ray<T>::GetOrigin() const
	{
		return myOrigin;
	}
	template<class T>
	inline void Ray<T>::SetDirection(const Vector3<T>& aDirection)
	{
		myDirection = aDirection.GetNormalized();
	}
	template<class T>
	inline void Ray<T>::SetOrigin(const Vector3<T>& aOrigin)
	{
		myOrigin = aOrigin;
	}
}