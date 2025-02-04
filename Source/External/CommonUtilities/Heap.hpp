#pragma once

#include <vector>
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Heap
	{
	public:
		Heap();
		~Heap();

		//Returns element count in heap
		int GetSize() const;

		//Adds elements to heap
		void Enqueue(const T& aElement);

		//Returns the top value in heap
		const T& GetTop() const;

		//Removes the top element in the heap and returns it
		T Dequeue();

	private:
		void Swap(size_t aIndex0, size_t aIndex1);

		std::vector<T> myVector;
	};

	template<class T>
	inline Heap<T>::Heap()
	{
	}

	template<class T>
	inline Heap<T>::~Heap()
	{
		myVector.~vector();
	}

	template<class T>
	inline int Heap<T>::GetSize() const
	{
		return myVector.size();
	}

	template<class T>
	inline void Heap<T>::Enqueue(const T& aElement)
	{
		myVector.push_back(aElement);

		if (myVector.size() <= 1)
		{
			return;
		}

		size_t index = myVector.size() - 1;
		while (index > 0)
		{
			if (myVector[(index - 1) / 2] < myVector[index])
			{
				Swap((index - 1) / 2, index);
				index = (index - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}

	template<class T>
	inline const T& Heap<T>::GetTop() const
	{
		assert(myVector.size() > 0);

		return myVector[0];
	}

	template<class T>
	inline T Heap<T>::Dequeue()
	{
		assert(myVector.size() > 0);

		T returnValue = myVector[0];
		myVector[0] = myVector.back();
		myVector.pop_back();

		size_t index = 0;
		while (index < myVector.size())
		{
			if (myVector.size() <= (index * 2) + 1)
			{
				break;
			}
			else if (myVector.size() == (index * 2) + 2)	//Has only 1 child
			{
				Swap(index, (index * 2) + 1);
				break;
			}



			size_t biggerChildIndex = (index * 2) + 1;
			if (myVector[(index * 2) + 1] < myVector[(index * 2) + 2])
			{
				biggerChildIndex = (index * 2) + 2;
			}

			if (myVector[index] < myVector[biggerChildIndex])
			{
				Swap(index, biggerChildIndex);
				index = biggerChildIndex;
			}
		}


		return returnValue;
	}


	template<class T>
	inline void Heap<T>::Swap(size_t aIndex0, size_t aIndex1)
	{
		if (aIndex0 < 0 || aIndex1 < 0)
		{
			return;
		}

		T tempValue = myVector[aIndex0];
		myVector[aIndex0] = myVector[aIndex1];
		myVector[aIndex1] = tempValue;
	}
}
