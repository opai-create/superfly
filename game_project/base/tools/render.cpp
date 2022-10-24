#include "render.h"

#include "../sdk.h"
#include "../global_variables.h"

#include "../other/color.h"

namespace render
{
	namespace fonts
	{
		unsigned long menu = 0;
		unsigned long visuals = 0;
		unsigned long visuals_name = 0;
		unsigned long visuals_flags = 0;
		unsigned long visuals_large = 0;

		unsigned long new_font(const char* name, int size, int weight, unsigned int flags)
		{
			auto font = i::surface->font_create();
			i::surface->set_font_glyph_set(font, name, size, weight, NULL, NULL, flags);

			return font;
		}

		void reset()
		{
			menu = 0;
			visuals_name = 0;
			visuals = 0;
			visuals_flags = 0;
			visuals_large = 0;
		}

		void create()
		{
			menu = new_font("Verdana", 13, 500, antialias | dropshadow);
			visuals_name = visuals = visuals_flags = new_font("Small Fonts", 9, 100, outline);
			visuals_large = new_font("Tahoma", 14, 800, dropshadow);
		}
	}

	rect2d screen = {};

	void init()
	{
		log_console("Initalizing render data...\n");

		int w, h;
		i::engine->get_screen_size(w, h);

		screen = rect2d(w, h);

		fonts::reset();

		fonts::create();

		log_console(" \n");
	}

	// thanks infirms for implementation
	std::string wide_to_multi_byte(const std::wstring_view& wstr)
	{
		if (wstr.empty())
			return std::string();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);

		std::string out(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &out[0], size_needed, NULL, NULL);

		return out;
	}

	std::wstring multi_byte_to_wide(const std::string_view& str)
	{
		if (str.empty())
			return std::wstring();

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), NULL, 0);

		std::wstring out(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &out[0], size_needed);

		return out;
	}

	void text(int x, int y, const color& clr, unsigned long font, unsigned int flags, const std::string& message)
	{
		std::wstring buffer = multi_byte_to_wide(message);

		int width, height;
		i::surface->get_text_size(font, buffer.c_str(), width, height);

		if (flags & centered_x)
			x -= width / 2;

		if (flags & centered_y)
			y -= height / 2;

		i::surface->draw_set_text_font(font);
		i::surface->draw_set_text_color(clr);
		i::surface->draw_set_text_pos(x, y);
		i::surface->draw_print_text(buffer.c_str(), buffer.size());
	}

	vector2d text_width(unsigned int font, const std::string& message)
	{
		std::wstring buffer = multi_byte_to_wide(message);

		int width, height;
		i::surface->get_text_size(font, buffer.c_str(), width, height);

		return vector2d(width, height);
	}

	void line(int x, int y, int x1, int y1, const color& clr)
	{
		i::surface->draw_set_color(clr);
		i::surface->draw_line(x, y, x1, y1);
	}

	void rect(int x, int y, int w, int h, const color& clr)
	{
		i::surface->draw_set_color(clr);
		i::surface->draw_outlined_rect(x, y, x + w, y + h);
	}

	void rect_filled(int x, int y, int w, int h, const color& clr)
	{
		i::surface->draw_set_color(clr);
		i::surface->draw_filled_rect(x, y, x + w, y + h);
	}

	void circle(int x, int y, int radius, int segments, const color& clr)
	{
		i::surface->draw_set_color(clr);
		i::surface->draw_outlined_circle(x, y, radius, segments);
	}

	void circle_filled(int x, int y, int radius, const color& clr)
	{
		vertex vertices[10] = {};
		int n = 0;

		for (float i = 0.f; i <= 6.28f; i += math::deg_to_rad(36.f))
		{
			float total_x = radius * std::cos(i) + x;
			float total_y = radius * std::sin(i) + y;

			vertices[n] = vector2d(total_y, total_x);
			n++;
		}

		i::surface->draw_set_color(clr);
		i::surface->add_text_poly(10, vertices);
	}

	void sphere(vector3d origin, float radius, float angle, float scale, color clr)
	{
		std::vector< vertex > vertices{};

		float step = (1.f / radius) + math::deg_to_rad(angle);

		for (float lat{}; lat < (3.14f * scale); lat += step)
		{
			for (float lon{}; lon < 3.14f * 2.f; lon += step)
			{
				vector3d point
				{
					origin.x + (radius * std::sin(lat) * std::cos(lon)),
					origin.y + (radius * std::sin(lat) * std::sin(lon)),
					origin.z + (radius * std::cos(lat))
				};

				vector2d screen;
				if (world_to_screen(point, screen))
					vertices.emplace_back(screen);
			}

			if (vertices.empty())
				continue;

			i::surface->draw_set_color(clr);
			i::surface->add_text_poly(vertices.size(), vertices.data());
		}
	}

	void triangle(const vector2d& first, const vector2d& second, const vector2d& third, const color& clr)
	{
		const vertex vertices[3] = { first, second, third };

		i::surface->draw_set_color(clr);
		i::surface->add_text_poly_line(vertices, 3);
	}

	void triangle_filled(const vector2d& first, const vector2d& second, const vector2d& third, const color& clr)
	{
		const vertex vertices[3] = { first, second, third };

		i::surface->draw_set_color(clr);
		i::surface->add_text_poly(3, vertices);
	}

	// credits to: https://www.unknowncheats.me/forum/source-engine/107653-offscreen-esp.html
	bool screen_transform(const vector3d& source, vector2d& output)
	{
		const matrix3x4_t& world_to_screen = i::engine->world_to_screen_matrix();

		output.x = world_to_screen.mat[0][0] * source.x +
			world_to_screen.mat[0][1] * source.y +
			world_to_screen.mat[0][2] * source.z +
			world_to_screen.mat[0][3];

		output.y = world_to_screen.mat[1][0] * source.x +
			world_to_screen.mat[1][1] * source.y +
			world_to_screen.mat[1][2] * source.z +
			world_to_screen.mat[1][3];

		float w =
			world_to_screen.mat[2][0] * source.x +
			world_to_screen.mat[2][1] * source.y +
			world_to_screen.mat[2][2] * source.z +
			world_to_screen.mat[2][3];

		bool behind = false;
		if (w < 0.001f)
		{
			behind = true;

			float invw = -1.0f / w;
			output.x *= invw;
			output.y *= invw;
		}
		else
		{
			behind = false;

			float invw = 1.0f / w;
			output.x *= invw;
			output.y *= invw;
		}
		return behind;
	}

	bool world_to_screen(const vector3d& source, vector2d& output)
	{
		bool st = screen_transform(source, output);

		float x = screen.w / 2;
		float y = screen.h / 2;

		x += 0.5 * output.x * screen.w + 0.5;
		y -= 0.5 * output.y * screen.h + 0.5;

		output.x = x;
		output.y = y;

		if (output.x > screen.w || output.x < 0 || output.y > screen.h || output.y < 0 || st)
			return false;

		return true;
	}
}