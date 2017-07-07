#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <assert.h>

template<typename T>
class singleton
{
public:
	using type = T;
	using pointer = type*;

	singleton()
	{
		assert(s_instance == nullptr);
		s_instance = static_cast<pointer>(this);
	}

	~singleton()
	{
		s_instance = nullptr;
	}

	static pointer instance() { return s_instance; }

private:
	static pointer s_instance;
};

template<typename T>
typename singleton<T>::pointer singleton<T>::s_instance{ nullptr };

template<typename T>
typename singleton<T>::pointer instance()
{
	return singleton<T>::instance();
}

#endif // SINGLETON_HPP