#pragma once
#include <memory>

template<class T>
class strong_ptr
{
public:
	strong_ptr();
	~strong_ptr();
	strong_ptr(const std::weak_ptr<T>& aWeakPtr);
	strong_ptr(const std::shared_ptr<T>& aSharedPtr);

	//strong_ptr<T>& operator=(const std::weak_ptr<T>& aWeakPtr);
	std::weak_ptr<T>& operator=(const strong_ptr<T>& aStrongPtr);

	bool operator==(const strong_ptr<T>& aStrongPtr1);

	operator bool() const;

	__forceinline T* operator->() const;
	__forceinline T& operator*() const;

	std::weak_ptr<T>& GetWeak();
	std::shared_ptr<T> GetShared();

private:
	std::weak_ptr<T> myPtr;
};

template<class T>
strong_ptr<T>::strong_ptr()
{

}

template<class T>
strong_ptr<T>::~strong_ptr()
{

}

template<class T>
strong_ptr<T>::strong_ptr(const std::weak_ptr<T>& aWeakPtr)
{
	myPtr = aWeakPtr;
}

template<class T>
strong_ptr<T>::strong_ptr(const std::shared_ptr<T>& aSharedPtr)
{
	myPtr = aSharedPtr;
}

//template<class T>
//inline strong_ptr<T>& strong_ptr<T>::operator=(const std::weak_ptr<T>& aWeakPtr)
//{
//	myPtr = aWeakPtr;
//
//	return *this;
//}

template<class T>
std::weak_ptr<T>& strong_ptr<T>::operator=(const strong_ptr<T>& aStrongPtr)
{
	myPtr = aStrongPtr.myPtr;
	return myPtr;
}

template<class T>
bool strong_ptr<T>::operator==(const strong_ptr<T>& aStrongPtr0)
{
	if (myPtr.expired() || aStrongPtr0.myPtr.expired())
	{
		throw std::exception("Pointer reference not set to an instance of an object");
	}

	return myPtr.lock().get() == aStrongPtr0.myPtr.lock().get();
}

template<class T>
inline strong_ptr<T>::operator bool() const
{
	return !myPtr.expired();
}

template<class T>
inline T* strong_ptr<T>::operator->() const
{
	if (myPtr.expired())
	{
		throw std::exception("Pointer reference not set to an instance of an object");
	}

	return myPtr.lock().get();
}

template<class T>
inline T& strong_ptr<T>::operator*() const
{
	return *myPtr.lock();
}

template<class T>
std::weak_ptr<T>& strong_ptr<T>::GetWeak()
{
	return myPtr;
}

template<class T>
std::shared_ptr<T> strong_ptr<T>::GetShared()
{
	return myPtr.lock();
}
