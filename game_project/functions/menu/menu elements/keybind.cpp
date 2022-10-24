#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

#include <unordered_map>

const std::vector<std::string> key_strings = {
	"no key", "left mouse", "right mouse", "control+break", "middle mouse", "mouse 4", "mouse 5",
	"unk", "backspace", "tab", "unk", "unk", "unk", "enter", "unk", "unk", "shift", "ctrl", "alt", "pause",
	"caps lock", "unk", "unk", "unk", "unk", "unk", "unk", "esc", "unk", "unk", "unk", "unk", "spacebar",
	"pg up", "pg down", "end", "home", "left", "up", "right", "down", "unk", "print", "unk", "print screen", "insert",
	"delete", "unk", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", "left windows", "right windows", "unk", "unk", "unk", "num 0", "num 1", "num 2", "num 3", "num 4", "num 5", "num 6",
	"num 7", "num 8", "num 9", "*", "+", "_", "-", ".", "/", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12",
	"f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "num lock", "scroll lock", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "lshift", "rshift", "lcontrol", "rcontrol", "lmenu", "rmenu", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "next track", "previous track", "stop", "play/pause", "unk", "unk",
	"unk", "unk", "unk", "unk", ";", "+", ",", "-", ".", "/?", "~", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "[{", "\\|", "}]", "'\"", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk","unk", "unk", "unk"
};

std::vector<const char*> types = {
	"always on",
	"hold",
	"toggle"
};

bool key_enabled(const key_bind_info_t& info)
{
	switch (info.type)
	{
	case 0:
		return true;
		break;
	case 1:
		return g::keys[info.key].down;
		break;
	case 2:
		return g::keys[info.key].pressed;
		break;
	}
	return false;
}

void key_bind(const char* name, key_bind_info_t& info, int idx, bool can_draw)
{
	const int add = (20 * idx);
	const int pos_y = menu_info.pos_y + 15 + add;
	const int size = 100;
	const int x = (menu_info.pos_x + menu_info.size_x) - (size + 15);
	const int default_x = menu_info.pos_x + menu_info.size_x;

	bool hovered_key_bind = can_draw && item_hovered(x, pos_y, size, 15);
	if (hovered_key_bind && g::keys[VK_LBUTTON].pressed)
		info.expand = !info.expand;

	// key bind's name
	render::text(menu_info.pos_x + 15, pos_y, color(), render::fonts::menu, 0, name);

	if (!can_draw)
		return;

	// key bind's body
	render::rect_filled(x, pos_y, size, 15, color(50, 50, 50));
	render::rect(x, pos_y, size, 15, color(60, 60, 60));

	// tell user to expand binder
	render::text(x + size / 2, pos_y, color(), render::fonts::menu, centered_x, "expand");

	if (info.expand)
	{
		const int new_pos_y = pos_y;
		const int new_size = 140;

		bool window_hovered = item_hovered(default_x + 10, new_pos_y, new_size, 100);

		render::rect_filled(default_x + 10, new_pos_y, new_size, 100, color(36, 36, 36));
		render::rect(default_x + 10, new_pos_y, new_size, 100, color(46, 46, 46));

		// key selector
		{
			const int key_x = default_x + 45;
			const int key_y = new_pos_y + 5;

			render::text(default_x + 15, key_y, color(), render::fonts::menu, 0, "key");

			// select key area
			// when you press on area will appear ... that indicates you should press any button
			// if any button pressed binder will save it
			// if pressed escape button binder resets value to zero

			// to-do: fix issues with left mouse button and make old key states
			bool hovered_binder_area = item_hovered(key_x, key_y, size, 15);

			render::rect_filled(key_x, key_y, size, 15, color(50, 50, 50));
			render::rect(key_x, key_y, size, 15, color(60, 60, 60));

			if (hovered_binder_area && g::keys[VK_LBUTTON].pressed)
				info.changing_key = !info.changing_key;

			if (info.changing_key && !g::keys[VK_LBUTTON].down)
			{
				for (int i = 0; i < 256; i++)
				{
					if (g::keys[VK_ESCAPE].pressed)
					{
						info.key = -1;
						info.changing_key = false;
					}
					else
					{
						if (g::keys[i].pressed)
						{
							info.key = i;
							info.changing_key = false;
						}
					}
				}
			}

			render::text(key_x + size / 2, key_y, color(), render::fonts::menu, centered_x,
				info.changing_key ? "..." : info.key == -1 ? "none" : key_strings[info.key].c_str());
		}

		// key type changer
		{
			const int key_x = default_x + 45;
			const int key_y = new_pos_y + 30;

			render::text(default_x + 15, key_y, color(), render::fonts::menu, 0, "type");

			bool hovered_type_changer = item_hovered(key_x, key_y, size, 15);

			if (hovered_type_changer && g::keys[VK_LBUTTON].pressed)
				info.changing_type = !info.changing_type;

			render::rect_filled(key_x, key_y, size, 15, color(50, 50, 50));
			render::rect(key_x, key_y, size, 15, color(60, 60, 60));

			if (info.changing_type)
			{
				// items body
				render::rect_filled(key_x, key_y + 20, size, 15 * types.size(), color(50, 50, 50));
				render::rect(key_x, key_y + 20, size, 15 * types.size(), color(60, 60, 60));

				for (size_t i = 0; i < types.size(); i++)
				{
					bool combo_item_hovered = item_hovered(key_x, key_y + 20 + (15 * i), size, 15);
					if (combo_item_hovered)
					{
						render::rect_filled(key_x, key_y + 20 + (15 * i), size, 15, color(60, 60, 60));

						if (g::keys[VK_LBUTTON].pressed)
						{
							info.type = i;
							info.changing_type = false;
						}
					}
					else
					{
						if (!hovered_type_changer && g::keys[VK_LBUTTON].pressed && info.expand)
							info.changing_type = false;
					}

					const color selected_clr = info.type == i ? color(255, 133, 139) : color();
					render::text(key_x + size / 2, key_y + 20 + (15 * i),
						selected_clr, render::fonts::menu, centered_x, types[i]);
				}
			}

			render::text(key_x + size / 2, key_y, color(), render::fonts::menu, centered_x, info.type == -1 ?
				"none" : types[info.type]);
		}

		if (!(hovered_key_bind || window_hovered) && g::keys[VK_LBUTTON].pressed)
			info.expand = false;
	}
}