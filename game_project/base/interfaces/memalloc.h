#pragma once
#include "../tools/memory.h"

class c_memory_allocate
{
public:
	void* alloc(int size)
	{
		using fn = void*(__thiscall*)(void*, int);
		return memory::vfunc<fn>(this, 1)(this, size);
	}

	void* realloc(void* mem, int size)
	{
		using fn = void*(__thiscall*)(void*, void*, int);
		return memory::vfunc<fn>(this, 3)(this, mem, size);
	}

	void free(void* mem)
	{
		using fn = void(__thiscall*)(void*, void*);
		return memory::vfunc<fn>(this, 5)(this, mem);
	}
};