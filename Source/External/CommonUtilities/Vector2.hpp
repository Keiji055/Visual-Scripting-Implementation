#pragma once

#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;
		//Creates a null-vector
		Vector2<T>();


		//Creates a vector (aX, aY, aZ)
		Vector2<T>(const T& aX, const T& aY);


		//Copy constructor (compiler generated)
		Vector2<T>(const Vector2<T>& aVector) = default;


		//Assignment operator (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;


		//Destructor (compiler generated)
		~Vector2<T>() = default;


		//Implicit cast operator to any datatype, return initializer list of x, y, z
		template <class U> operator U() const;


		//Returns the negated vector
		Vector2<T> operator-() const;


		//Returns the squared length of the vector
		T LengthSqr() const;


		//Returns the length of the vector
		T Length() const;


		//Returns a normalized copy of this
		Vector2<T> GetNormalized() const;


		//Normalizes the vector
		void Normalize();


		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const;

		//Returns the projected vector
		static Vector2<T> Project(Vector2<T> aVector, Vector2<T> aProjectTo);
	};




	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
	}



	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
	}



	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}



	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}



	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVectormultiplied by 1 / aScalar)
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0);

		double inverse = 1 / static_cast<double>(aScalar);

		return Vector2<T>(static_cast<T>(aVector.x * inverse), static_cast<T>(aVector.y * inverse));

	}



	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}



	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}



	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}



	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0);

		double inverse = 1 / static_cast<double>(aScalar);

		aVector.x *= static_cast<T>(inverse);
		aVector.y *= static_cast<T>(inverse);
	}



	template<class T>
	inline Vector2<T>::Vector2()
	{
		x = NULL;
		y = NULL;
	}

	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}

	template<class T>
	template<class U>
	inline Vector2<T>::operator U() const
	{
		return { x,y };
	}


	template<class T>
	inline Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2<T>(-x, -y );
	}


	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return x * x + y * y;
	}


	template<class T>
	inline T Vector2<T>::Length() const
	{
		return static_cast<T>(sqrt(x * x + y * y));
	}


	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		assert(!(x == 0 && y == 0));

		double lengthMultiple = 1 / static_cast<double>(Length());

		return Vector2<T>(static_cast<T>(x * lengthMultiple), static_cast<T>(y * lengthMultiple));
	}


	template<class T>
	inline void Vector2<T>::Normalize()
	{
		assert(!(x == 0 && y == 0));

		double lengthMultiple = 1 / static_cast<double>(Length());

		x *= static_cast<T>(lengthMultiple);
		y *= static_cast<T>(lengthMultiple);
	}


	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y;
	}

	template<class T>
	inline Vector2<T> Vector2<T>::Project(Vector2<T> aVector, Vector2<T> aProjectTo)
	{
		return aProjectTo.GetNormalized() * aProjectTo.Dot(aVector);
	}
}