#pragma once

template <class T>
class IThreadChannel
{
public:
	IThreadChannel();
	virtual ~IThreadChannel();
	virtual T* takeSync() = 0;
	virtual void submitSync(T*) = 0;
};

template <class T>
IThreadChannel<T>::IThreadChannel()
{
}

template <class T>
IThreadChannel<T>::~IThreadChannel()
{
}
