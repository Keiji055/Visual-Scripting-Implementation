#pragma once

#include "Line.hpp"
#include <vector>

namespace CommonUtilities
{
	template<class T>
	class LineVolume
	{
	public:
		// Default constructor: empty LineVolume.
		LineVolume();

		// Constructor taking a list of Line that makes up the LineVolume.
		LineVolume(const std::vector<Line<T>>& aLineList);

		// Add a Line to the LineVolume.
		void AddLine(const Line<T>& aLine);

		// Returns whether a point is inside the LineVolume: it is inside when the point is on the line or on the side the normal is pointing away from for all the lines in the LineVolume.
		bool IsInside(const Vector2<T>& aPosition) const;

	private:
		std::vector<Line<T>> myLineList;
	};

	typedef LineVolume<float> LineVolumef;
	typedef LineVolume<double> LineVolumed;

	template<class T>
	inline LineVolume<T>::LineVolume()
	{
	}

	template<class T>
	inline LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myLineList = aLineList;
	}

	template<class T>
	inline void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myLineList.push_back(aLine);
	}

	template<class T>
	inline bool LineVolume<T>::IsInside(const Vector2<T>& aPosition) const
	{
		for each (const Line<T>& line in myLineList)
		{
			if (!line.IsInside(aPosition))
			{
				return false;
			}
		}
		return true;
	}
}