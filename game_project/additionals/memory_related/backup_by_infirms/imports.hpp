#pragma once

#include <Windows.h>

class Imports {
public:
	using VirtualQuery_t = SIZE_T(__stdcall*)(LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T);
	VirtualQuery_t VirtualQuery;
};

inline Imports imports;