#pragma once

#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class AABB3D
	{
	public:
		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB3D();

		// Copy constructor.
		AABB3D(const AABB3D<T>& aAABB3D);

		// Constructor taking the positions of the minimum and maximum corners.
		AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);

		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);

		Vector3<T> GetMin() const;
		Vector3<T> GetMax() const;

		// Returns whether a point is inside the AABB: it is inside when the point is on one of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector3<T>& aPosition) const;

	private:
		union
		{
			Vector3<T> min;
			Vector3<T> myMin;
		};
		union
		{
			Vector3<T> max;
			Vector3<T> myMax;
		};

	};

	typedef AABB3D<float> AABB3Df;
	typedef AABB3D<double> AABB3Dd;

	template<class T>
	inline AABB3D<T>::AABB3D()
	{
		min = Vector3<T>();
		max = Vector3<T>();
	}

	template<class T>
	inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
	{
		min = aAABB3D.min;
		max = aAABB3D.max;
	}

	template<class T>
	inline AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		min = aMin;
		max = aMax;
	}

	template<class T>
	inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		min = aMin;
		max = aMax;
	}

	template<class T>
	inline Vector3<T> AABB3D<T>::GetMin() const
	{
		return min;
	}

	template<class T>
	inline Vector3<T> AABB3D<T>::GetMax() const
	{
		return max;
	}

	template<class T>
	inline bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return aPosition.x >= min.x && aPosition.y >= min.y && aPosition.x <= max.x && aPosition.y <= max.y && aPosition.z >= min.z && aPosition.z <= max.z;
	}
}