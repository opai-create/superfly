#pragma once
#include "../tools/memory.h"

class c_panel
{
public:
	const char* get_name(int iIndex)
	{
		using fn = const char*(__thiscall*)(void*, int);
		return memory::vfunc<fn>(this, 36)(this, iIndex);
	}

	void set_mouse_input_enabled(unsigned int panel, bool state)
	{
		using fn = void(__thiscall*)(PVOID, int, bool);
		return memory::vfunc<fn>(this, 32)(this, panel, state);
	}
};