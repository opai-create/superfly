#pragma once
#include "../tools/memory.h"

#include "../other/color.h"

#include "../ingame objects/c_utlvector.h"

#include <string>

// https://github.com/opai1337/supremacy/blob/b18d3716191000115cd3e353a30d496a30690586/cvar.h
using change_vallback_v1_fn = void(__cdecl*)();
using change_vallback_fn = void(__cdecl*)(void* var, const char* old, float flold);

struct convar_value_t
{
	char* string;
	int str_len;
	float flt;
	int nt;
};

class c_cvar
{
public:
	const char* get_string()
	{
		using fn = const char* (__thiscall*)(void*);
		return memory::vfunc<fn>(this, 11)(this);
	}

	float get_float()
	{
		using fn = float(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 12)(this);
	}

	int get_int()
	{
		using fn = int(__thiscall*)(void*);
		return memory::vfunc<fn>(this, 13)(this);
	}

	void set_value(const char* value)
	{
		using fn = void(__thiscall*)(void*, const char*);
		return memory::vfunc<fn>(this, 14)(this, value);
	}

	void set_value(float value)
	{
		using fn = void(__thiscall*)(void*, float);
		return memory::vfunc<fn>(this, 15)(this, value);
	}

	void set_value(int value)
	{
		using fn = void(__thiscall*)(void*, int);
		return memory::vfunc<fn>(this, 16)(this, value);
	}

private:
	char pad_0x0000[0x4];

public:
	c_cvar* next;
	__int32 is_registered;
	char* name;
	char* help_string;
	__int32 flags;

private:
	char pad_0x0018[0x4];

public:
	c_cvar* parent;
	char* default_value;
	char* string;
	__int32 string_length;
	float float_value;
	__int32 numerical_value;
	__int32 has_min;
	float min;
	__int32 has_max;
	float max;
	c_utlvector<change_vallback_fn> callbacks;
};

class c_convar
{
public:
	void print_console_color(const color& clr, const std::string& str)
	{
		using fn = void(__thiscall*)(void*, const color&, const char*);
		return memory::vfunc<fn>(this, 25)(this, clr, str.c_str());
	}

	c_cvar* find_convar(const char* str)
	{
		using fn = c_cvar*(__thiscall*)(void*, const char*);
		return memory::vfunc<fn>(this, 16)(this, str);
	}
};