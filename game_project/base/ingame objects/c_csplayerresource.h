#pragma once
#include "../tools/netvar_parser.h"
#include <memory>

class c_csplayer_resource
{
public:
	ADD_VAR(c4_carrier, int, this, offsets::m_iPlayerC4)
	int& get_ping(int index) 
	{ 
		return *(int*)((uintptr_t)this + offsets::m_iPing + index * sizeof(int));
	};
};