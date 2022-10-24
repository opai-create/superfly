#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

#include <vector>
#include <unordered_map>

struct combo_info_t
{
	bool expand;

	combo_info_t() : expand(false) { }
};

std::unordered_map<const char*, combo_info_t> combo_info = {};

bool combo_box(const char* name, int* v, int idx, const std::vector<const char*>& items, bool can_draw)
{
	const int add = (20 * idx);
	const int y = menu_info.pos_y + 15 + add;
	const int size = 100;
	const int x = (menu_info.pos_x + menu_info.size_x) - (size + 15);

	bool hovered = item_hovered(x, y, size, 15);

	combo_info_t& info = combo_info[name];

	if (can_draw && hovered && g::keys[VK_LBUTTON].pressed)
		info.expand = !info.expand;

	// combo name
	render::text(menu_info.pos_x + 15, y, color(), render::fonts::menu, 0, name);

	if (!can_draw)
		return false;

	// combo body
	render::rect_filled(x, y, size, 15, color(50, 50, 50));
	render::rect(x, y, size, 15, color(60, 60, 60));

	// tell user to select items!
	render::text(x + size / 2, y, color(), render::fonts::menu, centered_x, *v == -1 ? "select" : items[*v]);

	// expand combo
	if (info.expand)
	{
		// items body
		render::rect_filled(x, y + 20, size, 15 * items.size(), color(50, 50, 50));
		render::rect(x, y + 20, size, 15 * items.size(), color(60, 60, 60));

		for (size_t i = 0; i < items.size(); i++)
		{
			bool combo_item_hovered = item_hovered(x, y + 20 + (15 * i), size, 15);
			if (combo_item_hovered)
			{
				render::rect_filled(x, y + 20 + (15 * i), size, 15, color(60, 60, 60));

				if (g::keys[VK_LBUTTON].pressed)
				{
					*v = i;
					info.expand = false;
				}
			}
			else
			{
				if (!hovered && g::keys[VK_LBUTTON].pressed && info.expand)
					info.expand = false;
			}

			const color selected_clr = *v == i ? color(255, 133, 139) : color();
			render::text(x + size / 2, y + 20 + (15 * i), selected_clr, render::fonts::menu, centered_x, items[i]);
		}
	}
	return info.expand;
}
