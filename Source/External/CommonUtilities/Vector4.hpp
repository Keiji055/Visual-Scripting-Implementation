#pragma once

#include <cassert>


namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;
		//Creates a null-vector
		Vector4<T>();


		//Creates a vector (aX, aY, aZ)
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);


		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector) = default;


		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;


		//Destructor (compiler generated)
		~Vector4<T>() = default;


		//Implicit cast operator to any datatype, return initializer list of x, y, z
		template <class U> operator U() const;


		//Returns the negated vector
		Vector4<T> operator-() const;


		//Returns the squared length of the vector
		T LengthSqr() const;


		//Returns the length of the vector
		T Length() const;


		//Returns a normalized copy of this
		Vector4<T> GetNormalized() const;


		//Normalizes the vector
		void Normalize();


		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const;

		//Returns the projected vector
		static Vector4<T> Project(Vector4<T> aVector, Vector4<T> aProjectTo);
	};




	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
	}



	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
	}



	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}



	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}



	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVectormultiplied by 1 / aScalar)
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0);

		double inverse = 1 / static_cast<double>(aScalar);

		return Vector4<T>(static_cast<T>(aVector.x * inverse), static_cast<T>(aVector.y * inverse), static_cast<T>(aVector.z * inverse), static_cast<T>(aVector.w * inverse));

	}



	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}



	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}



	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}



	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0);

		double inverse = 1 / static_cast<double>(aScalar);

		aVector.x *= static_cast<T>(inverse);
		aVector.y *= static_cast<T>(inverse);
		aVector.z *= static_cast<T>(inverse);
		aVector.w *= static_cast<T>(inverse);
	}



	template<class T>
	inline Vector4<T>::Vector4()
	{
		x = NULL;
		y = NULL;
		z = NULL;
		w = NULL;
	}

	template<class T>
	inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	template<class U>
	inline Vector4<T>::operator U() const
	{
		return { x,y,z,w };
	}


	template<class T>
	inline Vector4<T> Vector4<T>::operator-() const
	{
		return Vector4<T>(-x, -y, -z, -w);
	}


	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return x * x + y * y + z * z + w * w;
	}


	template<class T>
	inline T Vector4<T>::Length() const
	{
		return static_cast<T>(sqrt(x * x + y * y + z * z + w * w));
	}


	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		assert(!(x == 0 && y == 0 && z == 0 && w == 0));

		double lengthMultiple = 1 / static_cast<double>(Length());

		return Vector4<T>(static_cast<T>(x * lengthMultiple), static_cast<T>(y * lengthMultiple), static_cast<T>(z * lengthMultiple), static_cast<T>(w * lengthMultiple));
	}


	template<class T>
	inline void Vector4<T>::Normalize()
	{
		assert(!(x == 0 && y == 0 && z == 0 && w == 0));

		double lengthMultiple = 1 / static_cast<double>(Length());

		x *= static_cast<T>(lengthMultiple);
		y *= static_cast<T>(lengthMultiple);
		z *= static_cast<T>(lengthMultiple);
		w *= static_cast<T>(lengthMultiple);
	}


	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
	}

	template<class T>
	inline Vector4<T> Vector4<T>::Project(Vector4<T> aVector, Vector4<T> aProjectTo)
	{
		return aProjectTo.GetNormalized() * aProjectTo.Dot(aVector);
	}
}