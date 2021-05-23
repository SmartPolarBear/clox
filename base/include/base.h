#pragma once

namespace clox::base
{
template<typename T>
class singleton
{
public:
	static T& Instance();

	singleton(const singleton&) = delete;

	singleton& operator=(const singleton&) = delete;

protected:
	singleton()
	{
	}
};

template<typename T>
T& singleton<T>::Instance()
{
	static T inst{};
	return inst;
}

}