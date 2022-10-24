#pragma once
#include "../tools/memory.h"

class c_localize
{
public:
	wchar_t* find(const char* name)
	{
		using fn = wchar_t*(__thiscall*)(void*, const char*);
		return memory::vfunc<fn>(this, 11)(this, name);
	}
};