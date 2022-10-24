#pragma once
#include "../tools/memory.h"

class c_input_system
{
public:
	void enable_input(bool enable)
	{
		using fn = void(__thiscall*)(void*, bool);
		memory::vfunc<fn>(this, 2)(this, enable);
	}
};