#pragma once

#include "Vector3.hpp"
#include "Plane.hpp"
#include "AABB3D.hpp"
#include "OOBB3D.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "UtilityFunctions.hpp"
#include <array>
#include <algorithm>


#ifdef min
#undef min
#endif

namespace CommonUtilities
{
	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
	// the ray is in the plane, true is returned, if not, false is returned. If the ray
	// isn't parallel to the plane, the intersection point is stored in
	// aOutIntersectionPoint and true returned.
	template<class T>
	inline bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		T tilt = aPlane.myNormal.Dot(aRay.GetDirection());
		if (CommonUtilities::Abs(tilt) > 0)
		{
			//Distance from nearest point of of plane to ray's origin
			T distance = Vector3<T>(aPlane.myPosition - aRay.GetOrigin()).Dot(aPlane.GetNormal());

			T t = distance / tilt;

			//Will get intersect point even if the intersect is behind the ray
			aOutIntersectionPoint = aRay.GetOrigin() + (aRay.GetDirection() * t);
			return t >= 0;
		}
		return false;
	}

	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// A ray in one of the AABB's sides is counted as intersecting it.
	template<class T>
	inline bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay)
	{
		Vector3<T> min = aAABB.GetMin();
		Vector3<T> max = aAABB.GetMax();


		std::array<Plane<T>, 6> planes =
		{
			Plane<T>(min, Vector3<T>(min.x, max.y, min.z), Vector3<T>(max.x, min.y, min.z)),	//Back
			Plane<T>(max, Vector3<T>(max.x, min.y, max.z), Vector3<T>(min.x, min.y, max.z)),	//Front
			Plane<T>(min, Vector3<T>(max.x, min.y, min.z), Vector3<T>(min.x, min.y, max.z)),	//Bottom
			Plane<T>(max, Vector3<T>(max.x, max.y, min.z), Vector3<T>(min.x, max.y, min.z)),	//Top
			Plane<T>(min, Vector3<T>(min.x, max.y, min.z), Vector3<T>(min.x, min.y, max.z)),	//Left
			Plane<T>(max, Vector3<T>(max.x, max.y, min.z), Vector3<T>(max.x, min.y, min.z))		//Right
		};

		Vector3<T> intersectPos;
		for (Plane<T>& plane : planes)
		{
			if (IntersectionPlaneRay(plane, aRay, intersectPos))
			{
				if (aAABB.IsInside(intersectPos))
				{
					return true; ///RTODO
				}
			}
		}

		return false;
	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// A rat intersecting the surface of the sphere is considered as intersecting it.
	template<class T>
	inline bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay)
	{
		return aSphere.IsInside(Vector3<T>::Project(aSphere.myCenter - aRay.GetOrigin(), aRay.GetDirection()) + aRay.GetOrigin());
	}

	//Returns true if an AABB is colliding with another AABB
	template<class T>
	inline bool IntersectionAABBAABB(const AABB3D<T>& aAABB1, const AABB3D<T>& aAABB2)
	{
		Vector3<T> aMax = aAABB1.GetMax();
		Vector3<T> aMin = aAABB1.GetMin();		
		Vector3<T> bMax = aAABB2.GetMax();
		Vector3<T> bMin = aAABB2.GetMin();

		return	!(aMax.x < bMin.x || aMin.x > bMax.x) &&	//To the left or right
				!(aMax.y < bMin.y || aMin.y > bMax.y) &&	//Above or below
				!(aMax.z < bMin.z || aMin.z > bMax.z);		//In front or behind			
	}

	template<class T>
	inline bool IntersectionSphereSphere(const Sphere<T>& aSphere1, const Sphere<T>& aSphere2)
	{
		return Vector3<T>(aSphere2.myCenter - aSphere1.myCenter).LengthSqr() < (aSphere1.myRadius * aSphere1.myRadius) + (aSphere2.myRadius * aSphere2.myRadius);
	}



	//Returns true if an OOBB is colliding with another OOBB
	template<class T>
	inline bool IntersectionOBBOOBB(const OOBB3D<T>& aOOBB1, const OOBB3D<T>& aOOBB2, T& aOutIntersectionDepth, int& aAxis, Vector3<T>& aOutNormal)
	{
		std::array<Vector3<T>, 15> separationAxes;

		//Face-vertex intersection
		separationAxes[0] = aOOBB1.GetPlanes()[0].GetNormal();
		separationAxes[1] = aOOBB1.GetPlanes()[1].GetNormal();
		separationAxes[2] = aOOBB1.GetPlanes()[2].GetNormal();

		separationAxes[3] = aOOBB2.GetPlanes()[3].GetNormal();
		separationAxes[4] = aOOBB2.GetPlanes()[4].GetNormal();
		separationAxes[5] = aOOBB2.GetPlanes()[5].GetNormal();

		//Edge-edge intersection
		separationAxes[6] = separationAxes[0].Cross(separationAxes[3]);
		separationAxes[7] = separationAxes[0].Cross(separationAxes[4]);
		separationAxes[8] = separationAxes[0].Cross(separationAxes[5]);

		separationAxes[9] = separationAxes[1].Cross(separationAxes[3]);
		separationAxes[10] = separationAxes[1].Cross(separationAxes[4]);
		separationAxes[11] = separationAxes[1].Cross(separationAxes[5]);

		separationAxes[12] = separationAxes[2].Cross(separationAxes[3]);
		separationAxes[13] = separationAxes[2].Cross(separationAxes[4]);
		separationAxes[14] = separationAxes[2].Cross(separationAxes[5]);

		T minDepth = T(INT_MAX);
		int minDepthAxis = 0;
		Vector3<T> normal = separationAxes[0];
		for (size_t i = 0; i < separationAxes.size(); i++)
		{
			const Vector3<T> axis = separationAxes[i];

			//axes 6-14, if the two sides are coplanar they return a 0 vector.
			//This means that axis is irrelevant and is already covered by another axis (I think)
			if (axis.x == T(0) && axis.y == T(0) && axis.z == T(0))
			{
				continue;
			}

			T oobb1Min = aOOBB1.GetVertices()[0].Dot(axis);
			T oobb1Max = aOOBB1.GetVertices()[0].Dot(axis);

			T oobb2Min = aOOBB2.GetVertices()[0].Dot(axis);
			T oobb2Max = aOOBB2.GetVertices()[0].Dot(axis);

			for (const Vector3<T>& i : aOOBB1.GetVertices())
			{
				T dotValue = i.Dot(axis);

				oobb1Min = std::min<T>(dotValue, oobb1Min);
				oobb1Max = std::max<T>(dotValue, oobb1Max);
			}

			for (const Vector3<T>& i : aOOBB2.GetVertices())
			{
				T dotValue = i.Dot(axis);

				oobb2Min = std::min<T>(dotValue, oobb2Min);
				oobb2Max = std::max<T>(dotValue, oobb2Max);
			}



			bool intersect = false;
			if (oobb1Max > oobb2Min && oobb1Min < oobb2Max)
			{
				intersect = true;

				if (oobb1Max - oobb2Min < minDepth)
				{
					minDepth = oobb1Max - oobb2Min;
					minDepthAxis = i;
					normal = axis;
				}
			}
			if (oobb2Max > oobb1Min && oobb2Min < oobb1Max)
			{
				intersect = true;

				if (oobb2Max - oobb1Min < minDepth)
				{
					minDepth = oobb2Max - oobb1Min;
					minDepthAxis = i;
					normal = -axis;
				}
			}


			//If there's no intersection, that means this is the axis/plane of separation
			if (!intersect)
			{
				aAxis = minDepthAxis;
				aOutIntersectionDepth = minDepth;
				aOutNormal = normal;
				return false;
			}
		}



		aAxis = minDepthAxis;
		aOutIntersectionDepth = minDepth;
		aOutNormal = normal;
		return true;
	}

	//Returns true if an OOBB is colliding with another OOBB
	template<class T>
	inline bool IntersectionOBBOOBB(const OOBB3D<T>& aOOBB1, const OOBB3D<T>& aOOBB2)
	{
		T data;
		int i;	
		Vector3<T> normal;
		return IntersectionOOBBOOBB(aOOBB1, aOOBB2, data, i, normal);
		/*
		std::array<Vector3<T>, 15> separationAxes;

		//Face-vertex intersection
		separationAxes[0] = aOOBB1.GetPlanes()[0].GetNormal();
		separationAxes[1] = aOOBB1.GetPlanes()[1].GetNormal();
		separationAxes[2] = aOOBB1.GetPlanes()[2].GetNormal();

		separationAxes[3] = aOOBB2.GetPlanes()[3].GetNormal();
		separationAxes[4] = aOOBB2.GetPlanes()[4].GetNormal();
		separationAxes[5] = aOOBB2.GetPlanes()[5].GetNormal();

		//Edge-edge intersection
		separationAxes[6] = separationAxes[0].Cross(separationAxes[3]);
		separationAxes[7] = separationAxes[0].Cross(separationAxes[4]);
		separationAxes[8] = separationAxes[0].Cross(separationAxes[5]);

		separationAxes[9] = separationAxes[1].Cross(separationAxes[3]);
		separationAxes[10] = separationAxes[1].Cross(separationAxes[4]);
		separationAxes[11] = separationAxes[1].Cross(separationAxes[5]);

		separationAxes[12] = separationAxes[2].Cross(separationAxes[3]);
		separationAxes[13] = separationAxes[2].Cross(separationAxes[4]);
		separationAxes[14] = separationAxes[2].Cross(separationAxes[5]);

		for (Vector3<T>& axis : separationAxes)
		{
			//axes 6-14, if the two sides are coplanar they return a 0 vector.
			//This means that axis is irrelevant and is already covered by another axis (I think)
			if (axis.x == T(0) && axis.y == T(0) && axis.z == T(0))
			{
				continue;
			}

			T oobb1Min = aOOBB1.GetVertices()[0].Dot(axis);
			T oobb1Max = aOOBB1.GetVertices()[0].Dot(axis);

			for (const Vector3<T>& i : aOOBB1.GetVertices())
			{
				T dotValue = i.Dot(axis);

				oobb1Min = std::min<T>(dotValue, oobb1Min);
				oobb1Max = std::max<T>(dotValue, oobb1Max);
			}



			T oobb2Min = aOOBB2.GetVertices()[0].Dot(axis);
			T oobb2Max = aOOBB2.GetVertices()[0].Dot(axis);

			for (const Vector3<T>& i : aOOBB2.GetVertices())
			{
				T dotValue = i.Dot(axis);

				oobb2Min = std::min<T>(dotValue, oobb2Min);
				oobb2Max = std::max<T>(dotValue, oobb2Max);
			}



			if (oobb1Max > oobb2Min && oobb1Min < oobb2Max)
			{
				continue;
			}
			else if (oobb2Max > oobb1Min && oobb2Min < oobb1Max)
			{
				continue;
			}

			//If there's no intersection, that means this is the axis/plane of separation
			return false;
		}

		//There's an intersection on all axis/planes, that means the two objects are colliding
		return true;*/
	}
}