#pragma once
#include <array>
#include <cassert>
#include <initializer_list>

namespace CommonUtilities
{
	template <typename T, int size, typename CountType = unsigned short, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		VectorOnStack();
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		VectorOnStack(const std::initializer_list<T>& aInitList);
		~VectorOnStack();

		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack);

		inline const T& operator[](const CountType aIndex) const;
		inline T& operator[](const CountType aIndex);

		inline void Add(const T& aObject);
		inline void Insert(const CountType aIndex, const T& aObject);

		inline void RemoveCyclic(const T& aObject);
		inline void RemoveCyclicAtIndex(const CountType aIndex);
		inline void Clear();

		__forceinline CountType Size() const;


	private:
		std::array<T, size> myArray;

		CountType myCurrentSize;
	};










	template <typename T, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<T, size, CountType, UseSafeModeFlag>::VectorOnStack()
	{
		assert(size > 0);

		myCurrentSize = 0;
	}

	template <typename T, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<T, size, CountType, UseSafeModeFlag>::VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
		assert(size > 0);

		myCurrentSize = aVectorOnStack.myCurrentSize;
		if (UseSafeModeFlag)
		{
			myArray = aVectorOnStack.myArray;
		}
		else
		{
			memcpy(&myArray, &aVectorOnStack.myArray, sizeof(aVectorOnStack.myArray));
		}
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<T, size, CountType, UseSafeModeFlag>::VectorOnStack(const std::initializer_list<T>& aInitList)
	{
		assert(aInitList.size() <= size);
		assert(aInitList.size() >= 0);

		myCurrentSize = 0;
		int index = 0;
		for (auto iterator = aInitList.begin(); iterator < aInitList.end(); iterator++)
		{
			myArray[index] = *iterator;
			index++;
			myCurrentSize++;
		}
	}

	template <typename T, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<T, size, CountType, UseSafeModeFlag>::~VectorOnStack()
	{
		myCurrentSize = 0;
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline VectorOnStack<T, size, CountType, UseSafeModeFlag>& VectorOnStack<T, size, CountType, UseSafeModeFlag>::operator=(const VectorOnStack& aVectorOnStack)
	{
		assert(size > 0);

		myArray = std::array<T, size>();
		myCurrentSize = aVectorOnStack.myCurrentSize;
		if (UseSafeModeFlag)
		{
			myArray = aVectorOnStack.myArray;
		}
		else
		{
			memcpy(&myArray, &aVectorOnStack.myArray, sizeof(aVectorOnStack.myArray));
		}

		return *this;
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline const T& VectorOnStack<T, size, CountType, UseSafeModeFlag>::operator[](const CountType aIndex) const
	{
		assert(aIndex >= 0);
		assert(aIndex < myCurrentSize);

		return myArray[aIndex];
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline T& VectorOnStack<T, size, CountType, UseSafeModeFlag>::operator[](const CountType aIndex)
	{
		assert(aIndex >= 0);
		assert(aIndex < myCurrentSize);

		return myArray[aIndex];
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, size, CountType, UseSafeModeFlag>::Add(const T& aObject)
	{
		assert(myCurrentSize < size);

		if (UseSafeModeFlag)
		{
			myArray[myCurrentSize] = aObject;
		}
		else
		{
			memcpy(&myArray[myCurrentSize], &aObject, sizeof(T));
		}


		myCurrentSize++;
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, size, CountType, UseSafeModeFlag>::Insert(const CountType aIndex, const T& aObject)
	{
		assert(myCurrentSize < size);
		assert(aIndex >= 0);
		assert(aIndex < size);
		assert(aIndex <= myCurrentSize);

		if (UseSafeModeFlag)
		{
			for (int i = myCurrentSize; i > aIndex; i--)
			{
				myArray[i] = myArray[i - 1];
			}

			myArray[aIndex] = aObject;
		}
		else
		{
			for (int i = myCurrentSize; i > aIndex; i--)
			{
				memcpy(&myArray[i], &myArray[i - 1], sizeof(T));
			}

			memcpy(&myArray[aIndex], &aObject, sizeof(T));
		}

		myCurrentSize++;
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, size, CountType, UseSafeModeFlag>::RemoveCyclic(const T& aObject)
	{
		int indexToRemoveAt = -1;
		for (int i = 0; i < myCurrentSize; i++)
		{
			if (myArray[i] == aObject)
			{
				indexToRemoveAt = i;
				break;
			}
		}

		if (indexToRemoveAt == -1)
		{
			return;
		}
		else
		{
			myArray[indexToRemoveAt] = myArray[myCurrentSize - 1];
			myCurrentSize--;
		}
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, size, CountType, UseSafeModeFlag>::RemoveCyclicAtIndex(const CountType aIndex)
	{
		assert(aIndex >= 0);
		assert(aIndex < myCurrentSize);

		myArray[aIndex] = myArray[myCurrentSize - 1];
		myCurrentSize--;
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	inline void VectorOnStack<T, size, CountType, UseSafeModeFlag>::Clear()
	{
		myCurrentSize = 0;
	}

	template<typename T, int size, typename CountType, bool UseSafeModeFlag>
	__forceinline CountType VectorOnStack<T, size, CountType, UseSafeModeFlag>::Size() const
	{
		return myCurrentSize;
	}	
}