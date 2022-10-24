#pragma once
#include "../../includes.h"

#include "../interfaces/surface.h"
#include "../tools/math.h"

enum center_flags : unsigned int
{
	centered_x = 1 << 0,
	centered_y = 1 << 1,
};

namespace render
{
	namespace fonts
	{
		extern unsigned long menu;
		extern unsigned long visuals;
		extern unsigned long visuals_name;
		extern unsigned long visuals_flags;
		extern unsigned long visuals_large;

		void create();
	}

	extern rect2d screen;
	void init();

	std::string wide_to_multi_byte(const std::wstring_view& str);
	std::wstring multi_byte_to_wide(const std::string_view& str);

	void text(int x, int y, const color& clr, unsigned long font, unsigned int flags, const std::string& message);
	vector2d text_width(unsigned int font, const std::string& message);

	void line(int x, int y, int x1, int y1, const color& clr);

	void rect(int x, int y, int w, int h, const color& clr);
	void rect_filled(int x, int y, int w, int h, const color& clr);

	void circle(int x, int y, int radius, int segments, const color& clr);
	void circle_filled(int x, int y, int radius, const color& clr);

	void triangle(const vector2d& first, const vector2d& second, const vector2d& third, const color& clr);
	void triangle_filled(const vector2d& first, const vector2d& second, const vector2d& third, const color& clr);

	bool world_to_screen(const vector3d& source, vector2d& output);
}