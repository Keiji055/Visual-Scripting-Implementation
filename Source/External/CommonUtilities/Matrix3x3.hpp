#pragma once

#include <array>
#include <cmath>
#include "Vector3.hpp"

namespace CommonUtilities
{
	template<class T>
	class Matrix4x4;

	template<class T>
	class Matrix3x3
	{
	public:
		// Creates the identity matrix.
		Matrix3x3<T>();

		// Copy Constructor.
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);

		// Copies the top left 3x3 part of the Matrix4x4.
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// Static functions for creating rotation matrices.
		static Matrix3x3<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(const T aAngleInRadians);

		static Matrix3x3<T> CreateScaleMatrix(Vector3<T> aScaleVector);

		// Static function for creating a transpose of a matrix.
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

		//Static function for getting the Determinant of a matrix.
		static T Determinant(const Matrix3x3<T>& aMatrix);

		//Static function for getting the Minor of an index in a matrix.
		static T MinorIndex(const Matrix3x3<T>& aMatrix, const int aRow, const int aColumn);

		// Static function for creating a Minor of a matrix.
		static Matrix3x3<T> Minor(const Matrix3x3<T>& aMatrix);

		// Static function for creating a Cofactor of a matrix.
		static Matrix3x3<T> Cofactor(const Matrix3x3<T>& aMatrix);

		// Static function for creating an Adjoint of a matrix.
		static Matrix3x3<T> Adjoint(const Matrix3x3<T>& aMatrix);

		//Static function for creating the Inverse of a matrix. Fails if the Determinant is 0.
		static bool TryInverse(Matrix3x3<T>& aMatrixToInverse);

		//Static function for creating the Inverse of a matrix. Returns identity matrix if Determinant is 0.
		static Matrix3x3<T> Inverse(const Matrix3x3<T>& aMatrixToInverse);

		//Creates a matrix from a vector which when multiplied by another vector gives the cross product of the two vectors.
		static Matrix3x3<T> Star(const Vector3<T>& aVector);



		//Returns a copy of the matrix, inversed.
		Matrix3x3<T> GetInverse() const;



		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T> operator*(const T& aScalar) const;
		Vector3<T> operator*(const Vector3<T>& aVector) const;

		Matrix3x3<T>& operator+=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator-=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator*=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator*=(const T& aScalar);

		bool operator==(const Matrix3x3<T>& aMatrix) const;
		bool operator!=(const Matrix3x3<T>& aMatrix) const;

	#pragma warning( disable :4201) // Nonstandard nameless struct/union.
		union
		{
			std::array<T, 9> myArray;
			
			std::array<std::array<T, 3>, 3> my2dArray;

			struct
			{
				Vector3<T> myXAxis;
				Vector3<T> myYAxis;
				Vector3<T> myZAxis;
			};

			Vector3<T> myRows[3];
		};
	#pragma warning( default :4201) // Nonstandard nameless struct/union.
	};
	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		myArray =
		{
			1,0,0,
			0,1,0,
			0,0,1
		};
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		myArray = aMatrix.myArray;
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		myArray =
		{
			aMatrix(1,1),
			aMatrix(1,2),
			aMatrix(1,3),

			aMatrix(2,1),
			aMatrix(2,2),
			aMatrix(2,3),

			aMatrix(3,1),
			aMatrix(3,2),
			aMatrix(3,3)
		};
	}

	template<class T>
	inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		return myArray[(aRow - 1) * 3 + (aColumn - 1)];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		return myArray[(aRow - 1) * 3 + (aColumn - 1)];
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);

		Matrix3x3 val;
		val.myArray =
		{
			T(1),	0,		0,
			0,		cos,	sin,
			0,		-sin,	cos
		};

		return val;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);

		Matrix3x3 val;
		val.myArray =
		{
			cos,	0,		-sin,
			0,		T(1),	0,
			sin,	0,		cos
		};

		return val;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);

		Matrix3x3 val;
		val.myArray =
		{
			cos,	sin,	0,
			-sin,	cos,	0,
			0,		0,		T(1)
		};

		return val;
	}

	template<typename T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateScaleMatrix(Vector3<T> aScaleVector)
	{
		Matrix3x3<T> result;
		result.myArray[0] = aScaleVector.x;
		result.myArray[4] = aScaleVector.y;
		result.myArray[8] = aScaleVector.z;
		return result;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3 val;
		val.myArray =
		{
			aMatrixToTranspose.myArray[0], aMatrixToTranspose.myArray[3], aMatrixToTranspose.myArray[6],
			aMatrixToTranspose.myArray[1], aMatrixToTranspose.myArray[4], aMatrixToTranspose.myArray[7],
			aMatrixToTranspose.myArray[2], aMatrixToTranspose.myArray[5], aMatrixToTranspose.myArray[8],
		};

		return val;
	}

	template<class T>
	inline T Matrix3x3<T>::Determinant(const Matrix3x3<T>& aMatrix)
	{
		//	abc
		//	pqr
		//	xyz
		// 
		//Det(a) = aqz + brx + cpy - ary - bpz - cqx

		return	aMatrix(1, 1) * aMatrix(2, 2) * aMatrix(3, 3) +
				aMatrix(1, 2) * aMatrix(2, 3) * aMatrix(3, 1) +
				aMatrix(1, 3) * aMatrix(2, 1) * aMatrix(3, 2) -
				aMatrix(1, 1) * aMatrix(2, 3) * aMatrix(3, 2) -
				aMatrix(1, 2) * aMatrix(2, 1) * aMatrix(3, 3) -
				aMatrix(1, 3) * aMatrix(2, 2) * aMatrix(3, 1);
	}

	template<class T>
	inline T Matrix3x3<T>::MinorIndex(const Matrix3x3<T>& aMatrix, const int aRow, const int aColumn)
	{
		//	abc
		//	def
		//	ghi
		// 
		//Minor(1, 1) (a) = ef - hi
		// you go down one row and right one column, and use that as the top left of a 2x2 matrix
		//The result is the determinant of the 2x2 matrix

		//range 1-3, if 4 then loops to 1
		T value11 = aMatrix((((aRow + 1) - 1) % 3) + 1, (((aColumn + 1) - 1) % 3) + 1);
		T value12 = aMatrix((((aRow + 1) - 1) % 3) + 1, (((aColumn + 2) - 1) % 3) + 1);
		T value21 = aMatrix((((aRow + 2) - 1) % 3) + 1, (((aColumn + 1) - 1) % 3) + 1);
		T value22 = aMatrix((((aRow + 2) - 1) % 3) + 1, (((aColumn + 2) - 1) % 3) + 1);

		return (value11 * value22) - (value21 * value12);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Minor(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> val;;

		const int myMatrixSize = 3;
		for (int i = 1; i <= myMatrixSize; i++)
		{
			for (int j = 1; j <= myMatrixSize; j++)
			{
				//alternating + and -, starting with +
				int sign;
				if ((i + j) % 2 == 0)
				{
					sign = 1;
				}
				else
				{
					sign = -1;
				}

				val(i, j) = Matrix3x3<T>::MinorIndex(aMatrix, i, j) * sign;
			}
		}

		return val;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Cofactor(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> val;
		const int myMatrixSize = 3;

		for (int i = 1; i <= myMatrixSize; i++)
		{
			for (int j = 1; j <= myMatrixSize; j++)
			{
				val(i, j) = Matrix3x3<T>::MinorIndex(aMatrix, i, j);
			}
		}

		return val;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Adjoint(const Matrix3x3<T>& aMatrix)
	{
		return Matrix3x3<T>::Transpose(Matrix3x3<T>::Cofactor(aMatrix));
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Star(const Vector3<T>& aVector)
	{
		Matrix3x3<T> val;
		val.myArray =
		{
			T(0),		-aVector.z,	aVector.y,
			aVector.z,	T(0),		-aVector.x,
			-aVector.y,	aVector.x,	T(0)
		};

		return val;
	}

	template<class T>
	inline bool Matrix3x3<T>::TryInverse(Matrix3x3<T>& aMatrixToInverse)
	{
		T determinant = Matrix3x3<T>::Determinant(aMatrixToInverse);
		if (determinant == T(0))
		{
			return false;
		}
		
		aMatrixToInverse = Matrix3x3<T>::Adjoint(aMatrixToInverse) * (1 / determinant);
		return true;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Inverse(const Matrix3x3<T>& aMatrixToInverse)
	{
		Matrix3x3<T> val = aMatrixToInverse;
		TryInverse(val);
		return val;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3 val;
		for (size_t i = 0; i < val.myArray.size(); i++)
		{
			val.myArray[i] = myArray[i] + aMatrix.myArray[i];
		}

		return val;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3 val;
		for (size_t i = 0; i < val.myArray.size(); i++)
		{
			val.myArray[i] = myArray[i] - aMatrix.myArray[i];
		}

		return val;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix) const
	{
		Matrix3x3 val;
		const int myMatrixSize = 3;

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
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const T& aScalar) const
	{
		Matrix3x3 val;

		for (size_t i = 0; i < myArray.size(); i++)
		{
			val.myArray[i] = myArray[i] * aScalar;
		}

		return val;
	}
	template<class T>
	inline Vector3<T> Matrix3x3<T>::operator*(const Vector3<T>& aVector) const
	{
		Vector3<T> vec;

		vec.x = (aVector.x * this->operator()(1, 1)) + (aVector.y * this->operator()(2, 1)) + (aVector.z * this->operator()(3, 1));
		vec.y = (aVector.x * this->operator()(1, 2)) + (aVector.y * this->operator()(2, 2)) + (aVector.z * this->operator()(3, 2));
		vec.z = (aVector.x * this->operator()(1, 3)) + (aVector.y * this->operator()(2, 3)) + (aVector.z * this->operator()(3, 3));

		return vec;
	}
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		return aMatrix * aVector;
	}



	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		*this = *this + aMatrix;

		return *this;
	}
	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		*this = *this - aMatrix;

		return *this;
	}
	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		*this = *this * aMatrix;

		return *this;
	}
	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const T& aScalar)
	{
		*this = *this * aScalar;

		return *this;
	}
	template<class T>
	inline bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aMatrix) const
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
	inline bool Matrix3x3<T>::operator!=(const Matrix3x3<T>& aMatrix) const
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



	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::GetInverse() const
	{
		return Matrix3x3<T>::Inverse(*this);
	}
}