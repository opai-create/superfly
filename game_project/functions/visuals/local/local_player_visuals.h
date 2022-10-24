#pragma once
class c_view_setup;

#undef local

namespace visuals
{
	namespace local
	{
		void change_view(c_view_setup* setup);
		void view_after_orig();

		void draw_pt();

		void draw_fsn(int stage);
	}
}