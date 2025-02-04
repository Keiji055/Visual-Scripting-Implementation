#pragma once

#include <cassert>
#include <cmath>

namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;
		//Creates a null-vector
		Vector3<T>();


		//Creates a vector (aX, aY, aZ)
		constexpr Vector3<T>(const T& aX, const T& aY, const T& aZ);

		//Creates a vector (all set to same value)
		Vector3<T>(const T& aAll);

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;


		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;


		//Destructor (compiler generated)
		~Vector3<T>() = default;


		//Implicit cast operator to any datatype, return initializer list of x, y, z
		template <class U> operator U() const;


		//Returns the negated vector
		Vector3<T> operator-() const;


		//Returns the squared length of the vector
		T LengthSqr() const;


		//Returns the length of the vector
		T Length() const;


		//Returns a normalized copy of this
		Vector3<T> GetNormalized() const;


		//Normalizes the vector
		void Normalize();


		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const;

		//Returns the projected vector
		static Vector3<T> Project(Vector3<T> aVector, Vector3<T> aProjectTo);

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const;





		static const Vector3<T> zero;
		static const Vector3<T> one;
		static const Vector3<T> forward;
		static const Vector3<T> back;
		static const Vector3<T> right;
		static const Vector3<T> left;
		static const Vector3<T> up;
		static const Vector3<T> down;
	};



	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
	}



	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
	}



	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar) 
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}


	//Returns the vector aVector0 multiplied by the vector aVector1
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z);
	}



	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector) 
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}



	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVectormultiplied by 1 / aScalar)
	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar) 
	{
		assert(aScalar != 0);

		double inverse = 1 / static_cast<double>(aScalar);

		return Vector3<T>(static_cast<T>(aVector.x * inverse), static_cast<T>(aVector.y * inverse), static_cast<T>(aVector.z * inverse));

	}

	template <class T> bool operator==(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return (aVector0.x == aVector1.x &&
				aVector0.y == aVector1.y &&
				aVector0.z == aVector1.z);
	}



	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}



	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}



	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar) 
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}



	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar) 
	{
		assert(aScalar != 0);

		double inverse = 1 / static_cast<double>(aScalar);

		aVector.x *= static_cast<T>(inverse);
		aVector.y *= static_cast<T>(inverse);
		aVector.z *= static_cast<T>(inverse);
	}



	template<class T>
	inline Vector3<T>::Vector3()
	{
		x = NULL;
		y = NULL;
		z = NULL;
	}

	template<class T>
	inline constexpr Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ) : x(aX), y(aY), z(aZ)
	{

	}

	template<class T>
	inline Vector3<T>::Vector3(const T& aAll)
	{
		x = aAll;
		y = aAll;
		z = aAll;
	}

	template<class T>
	template<class U>
	inline Vector3<T>::operator U() const
	{
		return { x,y,z };
	}


	template<class T>
	inline Vector3<T> Vector3<T>::operator-() const
	{
		return Vector3<T>(-x,-y,-z);
	}


	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return x * x + y * y + z * z;
	}


	template<class T>
	inline T Vector3<T>::Length() const
	{
		return static_cast<T>(std::sqrt(x * x + y * y + z * z));
	}


	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		if (x == 0 && y == 0 && z == 0)
		{
			return Vector3<T>::zero;
		}

		double lengthMultiple = 1 / static_cast<double>(Length());

		return Vector3<T>(static_cast<T>(x * lengthMultiple), static_cast<T>(y * lengthMultiple), static_cast<T>(z * lengthMultiple));
	}


	template<class T>
	inline void Vector3<T>::Normalize()
	{
		assert(!(x == 0 && y == 0 && z == 0));

		double lengthMultiple = 1 / static_cast<double>(Length());

		x *= static_cast<T>(lengthMultiple);
		y *= static_cast<T>(lengthMultiple);
		z *= static_cast<T>(lengthMultiple);
	}


	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z;
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Project(Vector3<T> aVector, Vector3<T> aProjectTo)
	{
		return aProjectTo.GetNormalized() * aProjectTo.Dot(aVector);
	}


	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return Vector3<T>(	y*aVector.z - z*aVector.y,
							z*aVector.x - x*aVector.z,
							x*aVector.y - y*aVector.x	);
	}




	template<typename T> const Vector3<T> Vector3<T>::zero = Vector3<T>(T(0), T(0), T(0));
	template<typename T> const Vector3<T> Vector3<T>::one = Vector3<T>(T(1), T(1), T(1));
	template<typename T> const Vector3<T> Vector3<T>::forward = Vector3<T>(T(0), T(0), T(1));
	template<typename T> const Vector3<T> Vector3<T>::back = Vector3<T>(T(0), T(0), T(-1));
	template<typename T> const Vector3<T> Vector3<T>::right = Vector3<T>(T(1), T(0), T(0));
	template<typename T> const Vector3<T> Vector3<T>::left = Vector3<T>(T(-1), T(0), T(0));
	template<typename T> const Vector3<T> Vector3<T>::up = Vector3<T>(T(0), T(1), T(0));
	template<typename T> const Vector3<T> Vector3<T>::down = Vector3<T>(T(0), T(-1), T(0));
}