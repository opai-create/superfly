#pragma once
class c_game_event;
class vector3d;
class color;

namespace visuals
{
	namespace events
	{
		namespace hit
		{
			void reset_info();
			void client_impact(int stage);
			void parse(c_game_event* event);
			void render();
		}

		namespace beams
		{
			void draw_beam(vector3d start, vector3d end, color clr);
		}
	}
}