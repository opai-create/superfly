#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

#include <algorithm>
#include <unordered_map>
#include <string>

std::unordered_map<const char*, int> slider_ticks = {};

void slider(const char* name, int* v, int min, int max, int idx, const char* item, bool can_draw)
{
	const int add = (20 * idx);
	const int y = menu_info.pos_y + 15 + add;
	const int size = 100;
	const int x = (menu_info.pos_x + menu_info.size_x) - (size + 15);

	float newpos = (float)std::clamp(cursor_pos[0] - x, 0, size);

	bool hovered_slider = can_draw && item_hovered(x, y, size, 15);

	bool hovered_minus = can_draw && item_hovered(x - 40, y, 15, 15);
	bool hovered_plus = can_draw && item_hovered(x - 20, y, 15, 15);

	if (g::keys[VK_LBUTTON].down)
		slider_ticks[name]++;
	else
		slider_ticks[name] = 0;

	bool press = g::keys[VK_LBUTTON].pressed || g::keys[VK_LBUTTON].down && slider_ticks[name] > 50;

	if (!hovered_slider)
	{
		if (hovered_plus && press)
			(*v)++;
		if (hovered_minus && press)
			(*v)--;
	}

	if (hovered_slider && g::keys[VK_LBUTTON].down)
	{
		float ratio = newpos / size;
		int val = (int)(min + (max - min) * ratio);
		*v = val;
	}

	(*v) = std::clamp((*v), min, max);

	float new_ratio = ((float)((*v) - min) / (float)(max - min));
	new_ratio = std::clamp(new_ratio, 0.f, 1.f);

	// slider's name
	render::text(menu_info.pos_x + 15, y, color(), render::fonts::menu, 0, name);

	if (!can_draw)
		return;

	// slider's body
	render::rect_filled(x, y, size, 15, color(50, 50, 50));

	// plus button
	render::rect_filled(x - 20, y, 15, 15, color(50, 50, 50));
	render::rect(x - 20, y, 15, 15, color(60, 60, 60));
	render::text(x - 13, y, color(), render::fonts::menu, centered_x, "+");

	// minus button
	render::rect_filled(x - 40, y, 15, 15, color(50, 50, 50));
	render::rect(x - 40, y, 15, 15, color(60, 60, 60));
	render::text(x - 33, y, color(), render::fonts::menu, centered_x, "-");

	// changed value
	render::rect_filled(x, y, new_ratio * size, 15, color(255, 133, 139));
	render::rect(x, y, size, 15, color(60, 60, 60));

	// value in string
	render::text(x + size / 2, y, color(), render::fonts::menu, centered_x, std::to_string(*v) + " " + item);
}
