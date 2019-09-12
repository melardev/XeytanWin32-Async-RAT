#pragma once


template <class T>
class IDoubleThreadChannel
{
public:
	IDoubleThreadChannel();
	virtual ~IDoubleThreadChannel();
protected:
	virtual void submitToLeft(T* event) = 0;
	virtual void submitToRight(T* event) = 0;
	virtual T* takeFromLeft() = 0;
	virtual T* takeFromRight() = 0;
};

template <class T>
IDoubleThreadChannel<T>::IDoubleThreadChannel()
{
}

template <class T>
IDoubleThreadChannel<T>::~IDoubleThreadChannel()
{
}
