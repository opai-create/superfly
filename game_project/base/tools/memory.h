#pragma once

#include "../../includes.h"

#include <array>

namespace m
{
	extern PE::Module client;
	extern PE::Module engine;
	extern PE::Module materialsystem;
	extern PE::Module tier0;

	void init();
}

namespace memory
{
	namespace pattern
	{
		void parse();
	}

	template <typename T = void*>
	inline T vfunc(void* ptr, std::size_t idx)
	{
		return (*static_cast<T**>(ptr))[idx];
	}

	void* get_interface(const char* library, const char* name);
}