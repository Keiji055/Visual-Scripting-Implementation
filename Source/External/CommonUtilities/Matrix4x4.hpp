#pragma once

#pragma once

#include <array>
#include <cmath>
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace CommonUtilities
{
	template<class T>
	class Matrix3x3;

	template<class T>
	class Matrix4x4
	{
	public:
		// Creates the identity matrix.
		Matrix4x4<T>();

		// Copy Constructor.
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		//Sets rotation part of matrix
		Matrix4x4<T>(const Matrix3x3<T>& aMatrix);

		Matrix4x4<T>(std::initializer_list<T> aList);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// Static functions for creating rotation matrices.
		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);

		// Static function for creating a transpose of a matrix.
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);

		// Assumes aTransform is made up of nothing but rotations and translations.
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);

		static Matrix4x4<T> CreateOrthographicProjection(T aLeftPlane, T aRightPlane, T aBottomPlane, T aTopPlane, T aNearPlane, T aFarPlane);
		static Matrix4x4<T> CreatePerspectiveProjection(const T aFov, const T aAspectRatio, const T aNearClip, const T aFarClip);

		static Matrix4x4<T> CreateScaleMatrix(Vector3<T> aScaleVector);
		static Matrix4x4<T> CreatePositionMatrix(Vector3<T> aPosition);


		//	template<class T>
		//inline Matrix4x4<T> Matrix4x4<T>::CreatePerspectiveMatrixFovX(const T aFov, const T aAspectRatio, const T aNearClip, const T aFarClip)
		//{
		//	float xScale = static_cast<T>(1) / tan(aFov / static_cast<T>(2));
		//	float yScale = xScale * aAspectRatio;

		//	Matrix4x4<T> persp;
		//	persp[0] = xScale;
		//	persp[5] = yScale;
		//	persp[10] = aFarClip / (aFarClip - aNearClip);
		//	persp[11] = T(1);
		//	persp[14] = (-aNearClip * aFarClip) / (aFarClip - aNearClip);
		//	persp[15] = 0;
		//	return persp;
		//}

		Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T> operator*(const T& aScalar) const;
		Vector4<T> operator*(const Vector4<T>& aVector) const;

		Matrix4x4<T>& operator+=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator-=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const T& aScalar);

		bool operator==(const Matrix4x4<T>& aMatrix) const;
		bool operator!=(const Matrix4x4<T>& aMatrix) const;


#pragma warning( disable :4201) // Nonstandard nameless struct/union.
		union
		{
			std::array<T, 16> myArray;

			struct
			{
				Vector4<T> myXAxis;
				Vector4<T> myYAxis;
				Vector4<T> myZAxis;
				Vector4<T> myPosition;
			};
		};
#pragma warning( default :4201) // Nonstandard nameless struct/union.

	private:

	};
	template<class T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		myArray =
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix3x3<T>& aMatrix)
	{
		myArray[0] = aMatrix.myArray[0];
		myArray[1] = aMatrix.myArray[1];
		myArray[2] = aMatrix.myArray[2];
		myArray[3] = T(0);

		myArray[4] = aMatrix.myArray[3];
		myArray[5] = aMatrix.myArray[4];
		myArray[6] = aMatrix.myArray[5];
		myArray[7] = T(0);

		myArray[8] = aMatrix.myArray[6];
		myArray[9] = aMatrix.myArray[7];
		myArray[10] = aMatrix.myArray[8];
		myArray[11] = T(0);

		myArray[12] = T(0);
		myArray[13] = T(0);
		myArray[14] = T(0);
		myArray[15] = T(1);
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		myArray = aMatrix.myArray;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(std::initializer_list<T> aList)
	{
		assert(aList.size() <= 16 && "Initializer list is to long.");
		std::memset(&myArray[0], 0, 16 * sizeof(T)); // starts matrix at 0 so if init list is not long enough then rest is 0
		std::memcpy(&myArray[0], aList.begin(), aList.size() * sizeof(T));
	}

	template<class T>
	inline T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		return myArray[(aRow - 1) * 4 + (aColumn - 1)];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		return myArray[(aRow - 1) * 4 + (aColumn - 1)];
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);

		Matrix4x4 val;
		val.myArray =
		{
			T(1),	0,		0,		0,
			0,		cos,	sin,	0,
			0,		-sin,	cos,	0,
			0,		0,		0,		T(1)
		};

		return val;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);

		Matrix4x4 val;
		val.myArray =
		{
			cos,	0,		-sin,	0,
			0,		T(1),	0,		0,
			sin,	0,		cos,	0,
			0,		0,		0,		T(1)

		};

		return val;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);

		Matrix4x4 val;
		val.myArray =
		{
			cos,	sin,	0,		0,
			-sin,	cos,	0,		0,
			0,		0,		T(1),	0,
			0,		0,		0,		T(1)

		};

		return val;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4 val;
		val.myArray =
		{
			aMatrixToTranspose.myArray[0], aMatrixToTranspose.myArray[4], aMatrixToTranspose.myArray[8], aMatrixToTranspose.myArray[12],
			aMatrixToTranspose.myArray[1], aMatrixToTranspose.myArray[5], aMatrixToTranspose.myArray[9], aMatrixToTranspose.myArray[13],
			aMatrixToTranspose.myArray[2], aMatrixToTranspose.myArray[6], aMatrixToTranspose.myArray[10], aMatrixToTranspose.myArray[14],
			aMatrixToTranspose.myArray[3], aMatrixToTranspose.myArray[7], aMatrixToTranspose.myArray[11], aMatrixToTranspose.myArray[15]
		};

		return val;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4<T> translation;
		translation(4, 1) = -aTransform(4, 1);
		translation(4, 2) = -aTransform(4, 2);
		translation(4, 3) = -aTransform(4, 3);


		Matrix4x4<T> rotation = aTransform;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;

		return translation * Transpose(rotation);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateOrthographicProjection(T aLeftPlane, T aRightPlane, T aBottomPlane, T aTopPlane, T aNearPlane, T aFarPlane)
	{
		const T reciprocalWidth = 1.0f / (aRightPlane - aLeftPlane);
		const T reciprocalHeight = 1.0f / (aTopPlane - aBottomPlane);
		const T fRange = 1.0f / (aFarPlane - aNearPlane);

		Matrix4x4<T> result;
		result(1, 1) = reciprocalWidth + reciprocalWidth;
		result(1, 2) = 0.0f;
		result(1, 3) = 0.0f;
		result(1, 4) = 0.0f;

		result(2, 1) = 0.0f;
		result(2, 2) = reciprocalHeight + reciprocalHeight;
		result(2, 3) = 0.0f;
		result(2, 4) = 0.0f;

		result(3, 1) = 0.0f;
		result(3, 2) = 0.0f;
		result(3, 3) = fRange;
		result(3, 4) = 0.0f;

		result(4, 1) = -(aLeftPlane + aRightPlane) * reciprocalWidth;
		result(4, 2) = -(aTopPlane + aBottomPlane) * reciprocalHeight;
		result(4, 3) = -fRange * aNearPlane;
		result(4, 4) = 1.0f;
		return result;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreatePerspectiveProjection(const T aFov, const T aAspectRatio, const T aNearClip, const T aFarClip)
	{
		float xScale = static_cast<T>(1) / tan(aFov / static_cast<T>(2));
		float yScale = xScale * aAspectRatio;

		Matrix4x4<T> persp;
		persp.myArray[0] = xScale;
		persp.myArray[5] = yScale;
		persp.myArray[10] = aFarClip / (aFarClip - aNearClip);
		persp.myArray[11] = T(1);
		persp.myArray[14] = (-aNearClip * aFarClip) / (aFarClip - aNearClip);
		persp.myArray[15] = 0;
		return persp;
	}



	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(Vector3<T> aScaleVector)
	{
		Matrix4x4<T> result;
		result.myArray[0] = aScaleVector.x;
		result.myArray[5] = aScaleVector.y;
		result.myArray[10] = aScaleVector.z;
		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreatePositionMatrix(Vector3<T> aPosition)
	{
		Matrix4x4<T> result;
		result(4, 1) = aPosition.x;
		result(4, 2) = aPosition.y;
		result(4, 3) = aPosition.z;
		return result;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4 val;
		for (size_t i = 0; i < val.myArray.size(); i++)
		{
			val.myArray[i] = myArray[i] + aMatrix.myArray[i];
		}

		return val;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4 val;
		for (size_t i = 0; i < val.myArray.size(); i++)
		{
			val.myArray[i] = myArray[i] - aMatrix.myArray[i];
		}

		return val;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix) const
	{
		Matrix4x4 val;
		int myMatrixSize = 4;

		for (int i = 1; i <= myMatrixSize; i++)
		{
			for (int j = 1; j <= myMatrixSize; j++)
			{
				T product(0);
				for (int k = 1; k <= myMatrixSize; k++)
				{
					product += this->operator()(i, k) * aMatrix(k, j);
				}
				val(i, j) = product;
			}
		}

		return val;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T& aScalar) const
	{
		Matrix4x4 val;

		for (size_t i = 0; i < myArray.size(); i++)
		{
			val.myArray[i] = myArray[i] * aScalar;
		}

		return val;
	}
	template<class T>
	inline Vector4<T> Matrix4x4<T>::operator*(const Vector4<T>& aVector) const
	{
		Vector4<T> vec = aVector;

		vec.x = (aVector.x * this->operator()(1, 1)) + (aVector.y * this->operator()(2, 1)) + (aVector.z * this->operator()(3, 1)) + (aVector.w * this->operator()(4, 1));
		vec.y = (aVector.x * this->operator()(1, 2)) + (aVector.y * this->operator()(2, 2)) + (aVector.z * this->operator()(3, 2)) + (aVector.w * this->operator()(4, 2));
		vec.z = (aVector.x * this->operator()(1, 3)) + (aVector.y * this->operator()(2, 3)) + (aVector.z * this->operator()(3, 3)) + (aVector.w * this->operator()(4, 3));
		vec.w = (aVector.x * this->operator()(1, 4)) + (aVector.y * this->operator()(2, 4)) + (aVector.z * this->operator()(3, 4)) + (aVector.w * this->operator()(4, 4));

		return vec;
	}
	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		return aMatrix * aVector;
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		*this = *this + aMatrix;

		return *this;
	}
	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		*this = *this - aMatrix;

		return *this;
	}
	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		*this = *this * aMatrix;

		return *this;
	}
	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const T& aScalar)
	{
		*this = *this * aScalar;

		return *this;
	}
	template<class T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
	{
		for (size_t i = 0; i < aMatrix.myArray.size(); i++)
		{
			if (myArray[i] != aMatrix.myArray[i])
			{
				return false;
			}
		}

		return true;
	}
	template<class T>
	inline bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& aMatrix) const
	{
		for (size_t i = 0; i < aMatrix.myArray.size(); i++)
		{
			if (myArray[i] != aMatrix.myArray[i])
			{
				return true;
			}
		}

		return false;
	}

}