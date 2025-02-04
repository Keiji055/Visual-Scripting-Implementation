#pragma once

#include "Vector2.hpp"

namespace CommonUtilities
{
	template <class T>
	class Line
	{
	public:
		// Default constructor: there is no line, the normal is the zero vector.
		Line();

		// Copy constructor.
		Line(const Line <T>& aLine);

		// Constructor that takes two points that define the line, the direction is aPoint1 -aPoint0.
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

		// Init the line with two points, the same as the constructor above.
		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

		// Init the line with a point and a direction.
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);

		// Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
		bool IsInside(const Vector2<T>&aPosition) const;

		// Returns the direction of the line.
		Vector2<T> GetDirection() const;

		// Returns the normal of the line, which is (-direction.y, direction.x), or to the left of the line
		const Vector2<T> GetNormal() const;

	private:
		Vector2<T> myPoint1;
		Vector2<T> myPoint2;
	};

	typedef Line<float> Linef;
	typedef Line<double> Lined;

	template<class T>
	inline Line<T>::Line()
	{
	}

	template<class T>
	inline Line<T>::Line(const Line<T>& aLine)
	{
		myPoint1 = aLine.myPoint1;
		myPoint2 = aLine.myPoint2;
	}

	template<class T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint1 = aPoint0;
		myPoint2 = aPoint1;
	}

	template<class T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint1 = aPoint0;
		myPoint2 = aPoint1;
	}

	template<class T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint1 = aPoint;
		myPoint2 = aPoint + aDirection;
	}

	template<class T>
	inline bool Line<T>::IsInside(const Vector2<T>& aPosition) const
	{
		return GetNormal().Dot(aPosition - myPoint1) <= 0;
	}

	template<class T>
	inline Vector2<T> Line<T>::GetDirection() const
	{
		return Vector2<T>(myPoint2 - myPoint1).GetNormalized();
	}

	template<class T>
	inline const Vector2<T> Line<T>::GetNormal() const
	{
		Vector2<T> dir = GetDirection();
		return Vector2<T>(-dir.y, dir.x);
	}
}