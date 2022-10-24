#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

void checkbox(const char* name, bool* v, int idx, bool can_draw)
{
	const int add = (20 * idx);
	const int default_x = menu_info.pos_x + menu_info.size_x;
	const int pos_y = menu_info.pos_y + 15 + add;

	bool hovered = item_hovered(default_x - 30, pos_y, 15, 15);

	bool triggered = can_draw && hovered && g::keys[VK_LBUTTON].pressed;
	if (triggered)
		*v = !*v;

	const color checkbox = *v ?
		color(255, 133, 139) : color(50, 50, 50);

	render::text(menu_info.pos_x + 15, pos_y, color(), render::fonts::menu, 0, name);

	if (!can_draw)
		return;

	render::rect_filled(default_x - 30, pos_y, 15, 15, checkbox);
	render::rect(default_x - 30, pos_y, 15, 15, color(60, 60, 60));
}