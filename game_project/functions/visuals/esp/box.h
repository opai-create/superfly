#pragma once
#include "../../../base/ingame objects/c_baseentity.h"

struct box_t
{
	float x, y, w, h;

	float alpha;
	bool valid;
	int class_id;

	box_t() : x(-1.f), y(-1.f), w(-1.f), h(-1.f), alpha(-1.f), valid(true), class_id(0) { }

	void get_box_bounds(c_baseentity* ent);

	box_t(c_baseentity* ent)
	{
		this->get_box_bounds(ent);
	}
};