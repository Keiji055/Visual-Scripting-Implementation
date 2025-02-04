#pragma once

#include "Matrix.hpp"
#include "Vector.hpp"

namespace CommonUtilities
{
#ifdef max
#undef max
#endif

	template <class T>
	class Quaternion
	{
	public:

		//w is the real part
		T w;
		T x;
		T y;
		T z;

		//Creates the identity quaternion
		Quaternion<T>();


		//Creates a Quaternion (aW, aX, aZ, aW)
		Quaternion<T>(const T& aW, const T& aX, const T& aY, const T& aZ);

		//Creates a Quaternion from a rotation matrix
		Quaternion<T>(const Matrix3x3<T> aMatrix);


		//Copy constructor (compiler generated)
		Quaternion<T>(const Quaternion<T>& aQuaternion) = default;

		//Destructor (compiler generated)
		~Quaternion<T>() = default;

		//Assignment operator (compiler generated)
		Quaternion<T>& operator=(const Quaternion<T>& aQuaternion) = default;

		//Returns the negated vector
		Quaternion<T> operator-() const;



		Quaternion<T> GetInversed() const;

		//Returns a normalized copy of this
		Quaternion<T> GetNormalized() const;


		Quaternion<T> LookAt(const Vector3f& sourcePoint,const Vector3f& destPoint) const;


		T GetLengthSqr() const;

		//Normalizes the quaternion
		void Normalize();

		//Inverses the quaternion. Assumes a rotation quaternion (inverse = conjugate)
		void Inverse();

		//Converts quaterion to rotation matrix
		Matrix3x3<T> ToMatrix() const;

		Matrix4x4<T> ToMatrix44() const;

		// Creates a quaternion, rotated about axis aAxis with aAngle degrees
		static Quaternion<T> AngleAxis(const Vector3<T>& aAxis, const T& aAngleDegrees);

		// Creates a quaternion, rotated about axis aAxis with aAngle radians
		static Quaternion<T> AngleAxisRad(const Vector3<T>& aAxis, const T& aAngleRadians);
	};

	//Returns the vector sum of aQuaternion0 and aQuaternion1
	template <class T> Quaternion<T> operator+(const Quaternion<T>& aQuaternion0, const Quaternion<T>& aQuaternion1)
	{
		return Quaternion<T>(aQuaternion0.w + aQuaternion1.w, aQuaternion0.x + aQuaternion1.x, aQuaternion0.y + aQuaternion1.y, aQuaternion0.z + aQuaternion1.z);
	}

	//Multiplies aQuaternion0 and aQuaternion1
	template <class T> Quaternion<T> operator*(const Quaternion<T>& aQuaternion0, const Quaternion<T>& aQuaternion1)
	{
		return Quaternion<T>(
			(aQuaternion1.w * aQuaternion0.w) - (aQuaternion1.x * aQuaternion0.x) - (aQuaternion1.y * aQuaternion0.y) - (aQuaternion1.z * aQuaternion0.z),
			(aQuaternion1.w * aQuaternion0.x) + (aQuaternion1.x * aQuaternion0.w) + (aQuaternion1.y * aQuaternion0.z) - (aQuaternion1.z * aQuaternion0.y),
			(aQuaternion1.w * aQuaternion0.y) + (aQuaternion1.y * aQuaternion0.w) + (aQuaternion1.z * aQuaternion0.x) - (aQuaternion1.x * aQuaternion0.z),
			(aQuaternion1.w * aQuaternion0.z) + (aQuaternion1.z * aQuaternion0.w) + (aQuaternion1.x * aQuaternion0.y) - (aQuaternion1.y * aQuaternion0.x));
	}

	//Returns the quaternion aQuaternion multiplied by the scalar aScalar
	template <class T> Quaternion<T> operator*(const Quaternion<T>& aQuaternion, const T& aScalar)
	{
		return Quaternion<T>(aQuaternion.w * aScalar, aQuaternion.x * aScalar, aQuaternion.y * aScalar, aQuaternion.z * aScalar);
	}

	//Rotates the vector aVector with quaternion aQuaternion. Encapsulates quaternion * vector * quaternion-inverse
	template <class T> Vector3<T> operator*(const Quaternion<T>& aQuaternion, const Vector3<T>& aVector)
	{
		// Extract the vector part of the quaternion
		Vector3<T> u(aQuaternion.x, aQuaternion.y, aQuaternion.z);

		Vector3<T> t = T(2) * u.Cross(aVector);
		Vector3<T> vprime = aVector + (aQuaternion.w * t) + u.Cross(t);

		return vprime;
	}



	//Returns the quaternion aQuaternion multiplied by the scalar aScalar
	template <class T> Quaternion<T> operator*(const T& aScalar, const Quaternion<T>& aQuaternion)
	{
		return Quaternion<T>(aQuaternion.w * aScalar, aQuaternion.x * aScalar, aQuaternion.y * aScalar, aQuaternion.z * aScalar);
	}

	//Equivalent to setting aQuaternion to (aQuaternion * aScalar)
	template <class T> void operator*=(Quaternion<T>& aQuaternion, const T& aScalar)
	{
		aQuaternion.w *= aScalar;
		aQuaternion.x *= aScalar;
		aQuaternion.y *= aScalar;
		aQuaternion.z *= aScalar;
	}

	//Equivalent to setting aQuaternion to (aQuaternion0 * aQuaternion1)
	template <class T> void operator*=(Quaternion<T>& aQuaternion0, const Quaternion<T>& aQuaternion1)
	{
		aQuaternion0 = aQuaternion0 * aQuaternion1;
	}

	template <class T> bool operator==(const Quaternion<T>& aQuaternion0, const Quaternion<T>& aQuaternion1)
	{
		return (aQuaternion0.x == aQuaternion1.x &&
				aQuaternion0.y == aQuaternion1.y &&
				aQuaternion0.z == aQuaternion1.z &&
				aQuaternion0.w == aQuaternion1.w);
	}

	template<class T>
	inline Quaternion<T>::Quaternion()
	{
		w = T(1);
		x = T(0);
		y = T(0);
		z = T(0);
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const T& aW, const T& aX, const T& aY, const T& aZ)
	{
		w = aW;
		x = aX;
		y = aY;
		z = aZ;
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Matrix3x3<T> aMatrix)
	{
		Matrix3x3f matrix = Matrix3x3f::Transpose(aMatrix);

		w = std::sqrt(std::max(T(0), T(1) + matrix(1, 1) + matrix(2, 2) + matrix(3, 3))) * T(0.5);
		x = std::sqrt(std::max(T(0), T(1) + matrix(1, 1) - matrix(2, 2) - matrix(3, 3))) * T(0.5);
		y = std::sqrt(std::max(T(0), T(1) - matrix(1, 1) + matrix(2, 2) - matrix(3, 3))) * T(0.5);
		z = std::sqrt(std::max(T(0), T(1) - matrix(1, 1) - matrix(2, 2) + matrix(3, 3))) * T(0.5);
		x = std::copysign(x, matrix(3, 2) - matrix(2, 3));
		y = std::copysign(y, matrix(1, 3) - matrix(3, 1));
		z = std::copysign(z, matrix(2, 1) - matrix(1, 2));
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::operator-() const
	{
		return Quaternion<T>(-w, -x, -y, -z);
	}
	template<class T>
	inline Quaternion<T> Quaternion<T>::GetInversed() const
	{
		return Quaternion<T>(w, -x, -y, -z) * (T(1) / GetLengthSqr());
	}
	template<class T>
	inline Quaternion<T> Quaternion<T>::GetNormalized() const
	{
		T dInv = T(1) / std::sqrt(w * w + x * x + y * y + z * z);

		return Quaternion<T>(w * dInv, x * dInv, y * dInv, z * dInv);
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::LookAt(const Vector3f& sourcePoint,const Vector3f& destPoint) const
	{
		Vector3f forwardVector = Vector3f(destPoint - sourcePoint).GetNormalized();

		Vector3f VectorForward = { 0,0,1 };
		Vector3f VectorUp = { 0,1,0 };

		float dot = VectorForward.Dot(forwardVector);

		if (std::abs(dot - (-1.0f)) < 0.000001f)
		{
			return Quaternion<T>(VectorUp.x, VectorUp.y, VectorUp.z, 3.1415926535897932f);
		}
		if (std::abs(dot - (1.0f)) < 0.000001f)
		{
			return Quaternion<T>();
		}

		VectorForward = { 0,0,1 };

		float rotAngle = (float)std::acos(dot);
		Vector3f rotAxis = VectorForward.Cross(forwardVector);
		rotAxis = rotAxis.GetNormalized();
		return AngleAxisRad(rotAxis, rotAngle);
	}

	template<class T>
	inline T Quaternion<T>::GetLengthSqr() const
	{
		return (w * w + x * x + y * y + z * z);
	}

	template<class T>
	inline void Quaternion<T>::Normalize()
	{
		T dInv = T(1) / std::sqrt(w * w + x * x + y * y + z * z);

		w *= dInv;
		x *= dInv;
		y *= dInv;
		z *= dInv;
	}

	template<class T>
	inline void Quaternion<T>::Inverse()
	{
		*this = this->GetInversed();
	}

	template<class T>
	inline Matrix3x3<T> Quaternion<T>::ToMatrix() const
	{
		Matrix3x3<T> result;
		T qxx(x * x);
		T qyy(y * y);
		T qzz(z * z);
		T qxz(x * z);
		T qxy(x * y);
		T qyz(y * z);
		T qwx(w * x);
		T qwy(w * y);
		T qwz(w * z);

		result(1, 1) = T(1) - T(2) * (qyy + qzz);
		result(2, 1) = T(2) * (qxy + qwz);
		result(3, 1) = T(2) * (qxz - qwy);

		result(1, 2) = T(2) * (qxy - qwz);
		result(2, 2) = T(1) - T(2) * (qxx + qzz);
		result(3, 2) = T(2) * (qyz + qwx);

		result(1, 3) = T(2) * (qxz + qwy);
		result(2, 3) = T(2) * (qyz - qwx);
		result(3, 3) = T(1) - T(2) * (qxx + qyy);
		return Matrix3x3f::Transpose(result);
	}

	template<class T>
	inline Matrix4x4<T> Quaternion<T>::ToMatrix44() const
	{
		return Matrix4x4<T>(ToMatrix());
	}



	template<class T>
	inline Quaternion<T> Quaternion<T>::AngleAxis(const Vector3<T>& aAxis, const T& aAngleDegrees)
	{
		//0.01745329251 = PI * 2 / 360
		return AngleAxisRad(aAxis, aAngleDegrees * T(0.01745329251));
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::AngleAxisRad(const Vector3<T>& aAxis, const T& aAngleRadians)
	{
		Vector3<T> axis = aAxis.GetNormalized();
		float s = std::sin(aAngleRadians * 0.5f);

		Quaternion<T> axisAngle;
		axisAngle.x = axis.x * s;
		axisAngle.y = axis.y * s;
		axisAngle.z = axis.z * s;
		axisAngle.w = std::cos(aAngleRadians * 0.5f);

		return axisAngle;
	}





	typedef Quaternion<float> Quatf;
	typedef Quaternion<double> Quatd;

	typedef Quaternion<float> Quaternionf;
	typedef Quaternion<double> Quaterniond;
}