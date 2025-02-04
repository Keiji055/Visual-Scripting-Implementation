#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template<class T>
	class Plane
	{
	public:
		// Default constructor.
		Plane();

		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 -aPoint0).
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);

		// Constructor taking a point and a normal.
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);

		// Init the plane with three points, the same as the constructor above.
		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, Vector3<T>& aPoint2);

		// Init the plane with a point and a normal, the same as the constructor above.
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

		// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
		bool IsInside(const Vector3<T>&aPosition) const;

		const Vector3<T>& GetNormal() const;
		const Vector3<T>& GetPosition() const;

		void SetNormal(const Vector3<T>& aNormal);
		void SetPosition(const Vector3<T>& aPosition);

	private:
		Vector3<T> myPosition;
		Vector3<T> myNormal;
	};

	typedef Plane<float> Planef;
	typedef Plane<double> Planed;

	template<class T>
	inline Plane<T>::Plane()
	{
	}

	template<class T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPosition = aPoint0;
		myNormal = Vector3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
	}

	template<class T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
	{
		myPosition = aPoint0;
		myNormal = aNormal;
	}

	template<class T>
	inline void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, Vector3<T>& aPoint2)
	{
		myPosition = aPoint0;
		myNormal = Vector3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
	}

	template<class T>
	inline void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myPosition = aPoint;
		myNormal = aNormal;
	}

	template<class T>
	inline bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPosition) < 0;
	}

	template<class T>
	inline const Vector3<T>& Plane<T>::GetNormal() const
	{
		return myNormal;
	}
	template<class T>
	inline const Vector3<T>& Plane<T>::GetPosition() const
	{
		return myPosition;
	}
	template<class T>
	inline void Plane<T>::SetNormal(const Vector3<T>& aNormal)
	{
		myNormal = aNormal.GetNormalized();
	}
	template<class T>
	inline void Plane<T>::SetPosition(const Vector3<T>& aPosition)
	{
		myPosition = aPosition;
	}
}