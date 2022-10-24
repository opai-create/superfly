#pragma once
#include <vector>
#include "../../base/other/color.h"

struct key_bind_info_t
{
	int key;
	int type;
	bool toggled;
	bool expand;
	bool changing_key;
	bool changing_type;

	key_bind_info_t() : key(-1), type(-1),
		toggled(false), expand(false), changing_key(false),
		changing_type(false)
	{ }
};

struct menu_info_t
{
public:
	int size_x, size_y;

	int pos_x, pos_y;

	inline void write_size(int w, int h)
	{
		size_x = w;
		size_y = h;
	}

	inline void write_pos(int x, int y)
	{
		pos_x = x;
		pos_y = y;
	}
};
extern menu_info_t menu_info;

// menu utils
extern int cursor_pos[2];
extern bool item_hovered(int x, int y, int w, int h);

// menu items
extern void checkbox(const char* name, bool* v, int idx, bool can_draw = true);
extern void slider(const char* name, int* v, int min, int max, int idx, const char* item, bool can_draw = true);
extern bool combo_box(const char* name, int* v, int idx, const std::vector<const char*>& items, bool can_draw = true);
extern bool multi_combo_box(const char* name, unsigned int* v, int idx, const std::vector<const char*>& items, bool can_draw = true);
extern void color_picker(const char* name, color* v, int idx, bool use_alpha = false, bool can_draw = true);
extern void key_bind(const char* name, key_bind_info_t& info, int idx, bool can_draw = true);
extern void button(const char* name, int idx, int pos, void(*callback)() = nullptr, bool can_draw = true);

extern void draw_menu();

extern void draw_cursor();