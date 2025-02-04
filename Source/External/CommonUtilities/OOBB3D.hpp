#pragma once

#include "Matrix.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Plane.hpp"

#include <array>

namespace CommonUtilities
{
	template <class T>
	class OOBB3D
	{
	public:
		// Default constructor: there is no OOBB, position and scale are 0
		OOBB3D();

		OOBB3D(const Vector3<T>& aPosition, const Quaternion<T>& aRotation, const Vector3<T>& aSize);

		// Returns whether a point is inside the OOBB3D: it is inside when the point is on one of the OOBB3D's sides or inside of the OOBB3D.
		bool IsInside(const Vector3<T>& aPosition) const;

		void SetPosition(const Vector3<T>& aPosition);
		void SetRotation(const Quaternion<T>& aRotation);
		void SetSize(const Vector3<T>& aSize);

		const Vector3<T>& GetPosition() const;
		const Quaternion<T>& GetRotation() const;
		const Vector3<T>& GetSize() const;

		const std::array<Plane<T>, 6>& GetPlanes() const;
		const std::array<Vector3<T>, 8>& GetVertices() const;

	private:
		void CreatePlanes();
		void CreateVertices();

		Vector3<T> myPosition;
		Quaternion<T> myRotation;
		Vector3<T> mySize;

		//First 3 values are all separate separation axis
		std::array<Plane<T>, 6> myPlanes;

		//Irrelevant order
		std::array<Vector3<T>, 8> myVertices;
		bool myVerticesDirty = true;
	};

	typedef OOBB3D<float> OOBB3Df;
	typedef OOBB3D<double> OOBB3Dd;

	template<class T>
	inline OOBB3D<T>::OOBB3D()
	{
		myPosition = Vector3<T>(T(0));
		mySize = Vector3<T>(T(0));
	}

	template<class T>
	inline OOBB3D<T>::OOBB3D(const Vector3<T>& aPosition, const Quaternion<T>& aRotation, const Vector3<T>& aSize)
	{
		myPosition = aPosition;
		myRotation = aRotation;
		mySize = aSize;

		CreatePlanes();
		CreateVertices();
	}

	template<class T>
	inline bool OOBB3D<T>::IsInside(const Vector3<T>& aPosition) const
	{
		for (const Plane<T>& plane : myPlanes)
		{
			if (!plane.IsInside(aPosition))
			{
				return false;
			}
		}
		return true;
	}

	template<class T>
	inline void OOBB3D<T>::SetPosition(const Vector3<T>& aPosition)
	{
		Vector3<T> positionChange = aPosition - myPosition;
		myPosition = aPosition;



		for (Plane<T>& plane : myPlanes)
		{
			plane.SetPosition(plane.GetPosition() + positionChange);
		}

		CreateVertices();
	}

	template<class T>
	inline void OOBB3D<T>::SetRotation(const Quaternion<T>& aRotation)
	{
		Quaternion<T> rotationChange = myRotation.GetInversed() * aRotation;
		myRotation = aRotation;



		for (Plane<T>& plane : myPlanes)
		{
			plane.SetNormal(rotationChange * plane.GetNormal());
		}

		CreateVertices();
	}

	template<class T>
	inline void OOBB3D<T>::SetSize(const Vector3<T>& aSize)
	{
		mySize = aSize;

		CreatePlanes();
		CreateVertices();
	}

	template<class T>
	inline const Vector3<T>& OOBB3D<T>::GetPosition() const
	{
		return myPosition;
	}

	template<class T>
	inline const Quaternion<T>& OOBB3D<T>::GetRotation() const
	{
		return myRotation;
	}

	template<class T>
	inline const Vector3<T>& OOBB3D<T>::GetSize() const
	{
		return mySize;
	}

	template<class T>
	inline const std::array<Plane<T>, 6>& OOBB3D<T>::GetPlanes() const
	{
		return myPlanes;
	}

	template<class T>
	inline const std::array<Vector3<T>, 8>& OOBB3D<T>::GetVertices() const
	{
		return myVertices;
	}

	template<class T>
	inline void OOBB3D<T>::CreatePlanes()
	{
		Matrix3x3<T> axis = myRotation.ToMatrix();

		myPlanes[0].InitWithPointAndNormal(myPosition + (axis.myXAxis * mySize.x), axis.myXAxis);
		myPlanes[1].InitWithPointAndNormal(myPosition + (axis.myYAxis * mySize.y), axis.myYAxis);
		myPlanes[2].InitWithPointAndNormal(myPosition + (axis.myZAxis * mySize.z), axis.myZAxis);

		myPlanes[3].InitWithPointAndNormal(myPosition - (axis.myXAxis * mySize.x), -axis.myXAxis);
		myPlanes[4].InitWithPointAndNormal(myPosition - (axis.myYAxis * mySize.y), -axis.myYAxis);
		myPlanes[5].InitWithPointAndNormal(myPosition - (axis.myZAxis * mySize.z), -axis.myZAxis);
	}
	template<class T>
	inline void OOBB3D<T>::CreateVertices()
	{
		Matrix3x3<T> axis = Matrix3x3<T>(Matrix4x4<T>::CreateScaleMatrix(mySize / T(2))) * myRotation.ToMatrix();

		//binary counting with the + and -
		myVertices[0] = myPosition + (axis.myXAxis + axis.myYAxis + axis.myZAxis);
		myVertices[1] = myPosition + (axis.myXAxis + axis.myYAxis - axis.myZAxis);
		myVertices[2] = myPosition + (axis.myXAxis - axis.myYAxis + axis.myZAxis);
		myVertices[3] = myPosition + (axis.myXAxis - axis.myYAxis - axis.myZAxis);
		myVertices[4] = myPosition + (-axis.myXAxis + axis.myYAxis + axis.myZAxis);
		myVertices[5] = myPosition + (-axis.myXAxis + axis.myYAxis - axis.myZAxis);
		myVertices[6] = myPosition + (-axis.myXAxis - axis.myYAxis + axis.myZAxis);
		myVertices[7] = myPosition + (-axis.myXAxis - axis.myYAxis - axis.myZAxis);
	}
}