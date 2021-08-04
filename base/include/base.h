#pragma once

#include <concepts>

namespace clox::base
{

template<typename T>
class singleton
{
public:
	static T& instance();

	singleton(const singleton&) = delete;

	singleton& operator=(const singleton&) = delete;

protected:
	singleton()
	{
	}
};

template<typename T>
T& singleton<T>::instance()
{
	static T inst{};
	return inst;
}


}