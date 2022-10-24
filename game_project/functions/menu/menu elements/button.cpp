#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

#include <unordered_map>
#include <thread>

std::unordered_map<const char*, bool> callback_toggled = {};

void button(const char* name, int idx, int pos, void(*callback)(), bool can_draw)
{
	const int add = (20 * idx);
	const int default_x = pos == 0 || pos == 2 ? menu_info.pos_x + 15
		: menu_info.pos_x + menu_info.size_x - 115;

	const int pos_y = menu_info.pos_y + 15 + add;

	const int size = pos == 2 ? 307 : 100;

	bool hovered_button = item_hovered(default_x, pos_y, size, 15);

	if (!can_draw)
		return;

	bool toggled = hovered_button && g::keys[VK_LBUTTON].pressed;
	if (callback)
	{
		if (toggled)
		{
			if (!callback_toggled[name])
			{
				std::thread(callback).detach();
				callback_toggled[name] = true;
			}
			toggled = false;
		}
		else
			callback_toggled[name] = false;
	}

	render::rect_filled(default_x, pos_y, size, 15, toggled ? color(60, 60, 60) : color(50, 50, 50));

	render::rect(default_x, pos_y, size, 15, color(60, 60, 60));

	render::text(default_x + size / 2, pos_y, color(), render::fonts::menu, centered_x, name);
}