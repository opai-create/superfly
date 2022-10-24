#pragma once
#include "../tools/memory.h"

struct studio_hdr_t;
struct model_t;

class c_model_info
{
public:
	studio_hdr_t* get_studio_model(const model_t* mod)
	{
		using fn = studio_hdr_t*(__thiscall*)(void*, const model_t*);
		return memory::vfunc<fn>(this, 32)(this, mod);
	}
};