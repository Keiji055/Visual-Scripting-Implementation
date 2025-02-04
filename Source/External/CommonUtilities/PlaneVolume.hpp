#pragma once

#include "Plane.hpp"
#include <vector>

namespace CommonUtilities
{
	template<class T>
	class PlaneVolume
	{
	public:
		// Default constructor: empty PlaneVolume.
		PlaneVolume();

		// Constructor taking a list of Plane that makes up the PlaneVolume.
		PlaneVolume(const std::vector<Plane<T>>& aPlaneList);

		// Add a Plane to the PlaneVolume.
		void AddPlane(const Plane<T>& aPlane);

		// Returns whether a point is inside the PlaneVolume: it is inside when the point is on plane or on the side the normal is pointing away from for all the planes in the PlaneVolume.
		bool IsInside(const Vector3<T>& aPosition) const;

	private:
		std::vector<Plane<T>> myPlaneList;
	};

	typedef PlaneVolume<float> PlaneVolumef;
	typedef PlaneVolume<double> PlaneVolumed;

	template<class T>
	inline PlaneVolume<T>::PlaneVolume()
	{
	}

	template<class T>
	inline PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
	{
		myPlaneList = aPlaneList;
	}

	template<class T>
	inline void PlaneVolume<T>::AddPlane(const Plane<T>& aPlane)
	{
		myPlaneList.push_back(aPlane);
	}

	template<class T>
	inline bool PlaneVolume<T>::IsInside(const Vector3<T>& aPosition) const
	{
		//for each (Plane<T>& plane in myPlaneList)
		//{
		//	if (!plane.IsInside(aPosition))
		//	{
		//		return false;
		//	}
		//}
		//return true;

		for (Plane<T>& plane : myPlaneList)
		{
			if (!plane.IsInside(aPosition))
			{
				return false;
			}
		}
		return true;
	}
}