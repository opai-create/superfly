#pragma once
#include "../../base/other/color.h"

class c_material;

namespace world_modulation
{
	struct fog_data_t
	{
		float start = 0.f;
		float end = 0.f;

		color primary_color = color(0, 0, 0);

		bool invalid()
		{
			return start == 0.f
				&& end == 0.f
				&& primary_color == color(0, 0, 0);
		}

		void reset()
		{
			start = end = 0.f;

			primary_color = color(0, 0, 0);
		}

		void init(float start, float end, const color& primary)
		{
			this->start = start;
			this->end = end;
			this->primary_color = primary;
		}
	};

	extern fog_data_t fog_data[4096];

	extern void change_world_clr(c_material* ptr, float* r, float* g, float* b);
	extern void change_clr(int a2, int a3, int a4, int a5, int a6, char a7);
	extern void start(int stage);
}