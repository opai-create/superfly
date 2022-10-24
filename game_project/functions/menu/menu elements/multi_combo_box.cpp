#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

#include <vector>
#include <unordered_map>

struct multi_combo_info_t
{
	bool expand;
	bool changed_value;

	multi_combo_info_t( ) : expand( false ), changed_value( false ) { }
};

std::unordered_map<const char*, multi_combo_info_t> multi_combo_info = {};

bool multi_combo_box(const char* name, unsigned int* v, int idx, const std::vector<const char*>& items, bool can_draw)
{
	const int add = (20 * idx);
	const int y = menu_info.pos_y + 15 + add;
	const int size = 100;
	const int x = (menu_info.pos_x + menu_info.size_x) - (size + 15);

	bool hovered = item_hovered(x, y, size, 15);

	multi_combo_info_t& info = multi_combo_info[name];

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
	render::text(x + size / 2, y, color(), render::fonts::menu, centered_x, "select");

	// expand combo
	if (info.expand)
	{
		// items body
		render::rect_filled(x, y + 20, size, 15 * items.size(), color(50, 50, 50));
		render::rect(x, y + 20, size, 15 * items.size(), color(60, 60, 60));

		bool expanded_box_hovered = item_hovered(x, y + 20, size, 15 * items.size());

		for (size_t i = 0; i < items.size(); i++)
		{
			bool combo_item_hovered = item_hovered(x, y + 20 + (15 * i), size, 15);
			if (combo_item_hovered)
				render::rect_filled(x, y + 20 + (15 * i), size, 15, color(60, 60, 60));

			if (combo_item_hovered)
			{
				if (g::keys[VK_LBUTTON].pressed && !info.changed_value)
				{
					*v |= (1 << i);
					info.changed_value = true;
				}
				else if (g::keys[VK_LBUTTON].pressed && info.changed_value)
				{
					*v &= ~(1 << i);
					info.changed_value = false;
				}
			}
			else
			{
				if (!(expanded_box_hovered || hovered) && g::keys[VK_LBUTTON].pressed && info.expand)
					info.expand = false;
			}

			const color selected_clr = *v & (1 << i) ? color(255, 133, 139) : color();
			render::text(x + size / 2, y + 20 + (15 * i), selected_clr, render::fonts::menu, centered_x, items[i]);
		}
	}
	return info.expand;
}