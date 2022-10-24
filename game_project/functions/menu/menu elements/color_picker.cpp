#include "../menu.h"

#include "../../../base/tools/render.h"
#include "../../../base/global_variables.h"

#include <string>
#include <unordered_map>

struct color_picker_info_t
{
	float hue;
	float saturation;
	float brightness;
	float alpha;

	color_picker_info_t() : hue(-1.f), saturation(-1.f), brightness(-1.f), alpha(-1.f) { }

	inline bool undefined()
	{
		return hue == -1.f
			&& saturation == -1.f
			&& brightness == -1.f
			&& alpha == -1.f;
	}

	inline void reset()
	{
		hue = saturation = brightness = alpha = -1.f;
	}

	bool changed()
	{
		if (undefined())
			return false;

		static float h, s, b, a;

		if (h != hue
			|| s != saturation
			|| b != brightness
			|| a != alpha)
		{
			h = hue;
			s = saturation;
			b = brightness;
			a = alpha;
			return true;
		}
		return false;
	}

	void init(float h, float s, float b, float a = 1.f)
	{
		hue = h;
		saturation = s;
		brightness = b;
		alpha = a;
	}
};

void to_clipboard(const char* text)
{
	if (OpenClipboard(0))
	{
		EmptyClipboard();
		char* clip_data = (char*)(GlobalAlloc(GMEM_FIXED, MAX_PATH));
		lstrcpy(clip_data, text);
		SetClipboardData(CF_TEXT, (HANDLE)(clip_data));
		LCID* lcid = (DWORD*)(GlobalAlloc(GMEM_FIXED, sizeof(DWORD)));
		*lcid = MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL), SORT_DEFAULT);
		SetClipboardData(CF_LOCALE, (HANDLE)(lcid));
		CloseClipboard();
	}
}

std::string from_clipboard()
{
	std::string fromClipboard = "";
	if (OpenClipboard(0))
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		char* chBuffer = (char*)GlobalLock(hData);
		fromClipboard = chBuffer;
		GlobalUnlock(hData);
		CloseClipboard();
	}
	return fromClipboard;
}

std::unordered_map<const char*, bool> expand_clr = {};
std::unordered_map<const char*, color_picker_info_t> color_info = {};

void color_picker(const char* name, color* v, int idx, bool use_alpha, bool can_draw)
{
	const int add = (20 * idx);
	const int pos_y = menu_info.pos_y + 15 + add;
	const int default_x = menu_info.pos_x + menu_info.size_x;

	bool hovered = item_hovered(default_x - 30, pos_y, 15, 15);

	if (can_draw && hovered && g::keys[VK_LBUTTON].pressed)
		expand_clr[name] = !expand_clr[name];

	color_picker_info_t& info = color_info[name];

	color preview = *v;

	auto create_slider = [&](float* v, int pos_x, int new_pos_y, int size)
	{
		float newpos = (float)std::clamp(cursor_pos[0] - pos_x, 0, size);
		bool hovered_slider = can_draw && item_hovered(pos_x, new_pos_y, size, 15);

		if (hovered_slider && g::keys[VK_LBUTTON].down)
		{
			float ratio = newpos / size;
			float val = (float)(0.f + (1.f - 0.f) * ratio);
			*v = val;
		}
	};

	const int slider_size = 126;
	if (expand_clr[name])
	{
		if (info.undefined())
		{
			info.init(
				preview.get_hue(),
				preview.get_saturation(),
				preview.get_brightness(),
				preview.a() / 255.f
			);
		}

		const int new_pos_y = pos_y;

		bool window_hovered = item_hovered(default_x + 10, new_pos_y, 150, 100);

		render::rect_filled(default_x + 10, new_pos_y, 150, 100, color(36, 36, 36));
		render::rect(default_x + 10, new_pos_y, 150, 100, color(46, 46, 46));

		// hue slider
		{
			const int hue_x = default_x + 28;
			const int hue_y = new_pos_y + 5;

			render::text(default_x + 15, new_pos_y + 5, color(), render::fonts::menu, 0, "H");

			create_slider(&info.hue, hue_x, hue_y, slider_size);
			float new_ratio = std::clamp(info.hue / 1.f, 0.f, 1.f);

			// slider body
			render::rect_filled(hue_x, hue_y, slider_size, 15, color::hsb(info.hue, 1.f, 1.f));
			render::rect(hue_x, hue_y, slider_size, 15, color(60, 60, 60));

			// slider selected bar
			int rect_pos = hue_x + ((slider_size * new_ratio) - 5);
			rect_pos = std::clamp(rect_pos, hue_x, (hue_x + slider_size) - 5);

			render::rect_filled(rect_pos, hue_y, 5, 15, color());
			render::rect(rect_pos, hue_y, 5, 15, color(60, 60, 60));
		}

		// saturation slider
		{
			const int saturation_x = default_x + 28;
			const int saturation_y = new_pos_y + 25;

			create_slider(&info.saturation, saturation_x, saturation_y, slider_size);
			float new_ratio = std::clamp(info.saturation / 1.f, 0.f, 1.f);

			render::text(default_x + 15, new_pos_y + 25, color(), render::fonts::menu, 0, "S");

			// slider body
			render::rect_filled(default_x + 28, new_pos_y + 25, 126, 15, color::hsb(info.hue, info.saturation, 1.f));
			render::rect(default_x + 28, new_pos_y + 25, 126, 15, color(60, 60, 60));

			// slider selected bar
			int rect_pos = saturation_x + ((slider_size * new_ratio) - 5);
			rect_pos = std::clamp(rect_pos, saturation_x, (saturation_x + slider_size) - 5);

			render::rect_filled(rect_pos, saturation_y, 5, 15, color());
			render::rect(rect_pos, saturation_y, 5, 15, color(60, 60, 60));
		}

		// value slider
		{
			const int value_x = default_x + 28;
			const int value_y = new_pos_y + 45;

			create_slider(&info.brightness, value_x, value_y, slider_size);
			float new_ratio = std::clamp(info.brightness / 1.f, 0.f, 1.f);;

			render::text(default_x + 15, new_pos_y + 45, color(), render::fonts::menu, 0, "V");

			// slider body
			render::rect_filled(default_x + 28, new_pos_y + 45, 126, 15, color::hsb(info.hue, info.saturation, info.brightness));
			render::rect(default_x + 28, new_pos_y + 45, 126, 15, color(60, 60, 60));

			// slider selected bar
			int rect_pos = value_x + ((slider_size * new_ratio) - 5);
			rect_pos = std::clamp(rect_pos, value_x, (value_x + slider_size) - 5);

			render::rect_filled(rect_pos, value_y, 5, 15, color());
			render::rect(rect_pos, value_y, 5, 15, color(60, 60, 60));
		}

		if (use_alpha)
		{
			// alpha slider
			{
				const int alpha_x = default_x + 28;
				const int alpha_y = new_pos_y + 65;

				create_slider(&info.alpha, alpha_x, alpha_y, slider_size);
				float new_ratio = std::clamp(info.alpha / 1.f, 0.f, 1.f);

				render::text(default_x + 15, new_pos_y + 65, color(), render::fonts::menu, 0, "A");

				// slider bar
				render::rect_filled(default_x + 28, new_pos_y + 65, 126, 15,
					color::hsb(info.hue, info.saturation, 1.f).manage_alpha(info.alpha * 255));
				render::rect(default_x + 28, new_pos_y + 65, 126, 15, color(60, 60, 60));

				// slider selected bar
				int rect_pos = alpha_x + ((slider_size * new_ratio) - 5);
				rect_pos = std::clamp(rect_pos, alpha_x, (alpha_x + slider_size) - 5);

				render::rect_filled(rect_pos, alpha_y, 5, 15, color());
				render::rect(rect_pos, alpha_y, 5, 15, color(60, 60, 60));
			}
		}

		static color clipboard_clr = color();
		int color_picker_add = use_alpha ? 85 : 70;
		// copy button
		{
			bool hovered_copy = item_hovered(default_x + 10, new_pos_y + color_picker_add, 70, 15);

			if (hovered_copy)
				render::rect_filled(default_x + 10, new_pos_y + color_picker_add, 70, 15, color(50, 50, 50, 150));

			render::text(default_x + 45, new_pos_y + color_picker_add, color(), render::fonts::menu, centered_x, "copy");

			if (hovered_copy && g::keys[VK_LBUTTON].pressed)
			{
				clipboard_clr =
					use_alpha ? color::hsb(info.hue, info.saturation, info.brightness).manage_alpha(info.alpha * 255)
					: color::hsb(info.hue, info.saturation, info.brightness).manage_alpha(v->a());

				std::string new_str = std::to_string(clipboard_clr.u32());
				to_clipboard(new_str.c_str());
				expand_clr[name] = false;
			}
		}

		// paste button
		{
			bool hovered_paste = item_hovered(default_x + 90, new_pos_y + color_picker_add, 70, 15);

			if (hovered_paste)
				render::rect_filled(default_x + 90, new_pos_y + color_picker_add, 70, 15, color(50, 50, 50, 150));

			render::text(default_x + 125, new_pos_y + color_picker_add, color(), render::fonts::menu, centered_x, "paste");

			if (hovered_paste && g::keys[VK_LBUTTON].pressed)
			{
				clipboard_clr.set_u32(std::atoll(from_clipboard().c_str()));

				info.init(
					clipboard_clr.get_hue(),
					clipboard_clr.get_saturation(),
					clipboard_clr.get_brightness(),
					clipboard_clr.a() / 255.f
				);

				expand_clr[name] = false;
			}
		}

		if (!(window_hovered || hovered) && g::keys[VK_LBUTTON].pressed)
			expand_clr[name] = false;
	}
	else
		info.reset();

	if (!info.undefined() && info.changed())
	{
		const color total_clr =
			use_alpha ? color::hsb(info.hue, info.saturation, info.brightness).manage_alpha(info.alpha * 255.f)
			: color::hsb(info.hue, info.saturation, info.brightness).manage_alpha(v->a());

		*v = total_clr;
	}

	render::text(menu_info.pos_x + 15, menu_info.pos_y + 15 + add, color(), render::fonts::menu, 0, name);

	if (!can_draw)
		return;

	render::rect_filled(default_x - 23, pos_y, 7, 15, *v);
	render::rect_filled(default_x - 30, pos_y, 7, 15, v->manage_alpha(255));

	render::rect(default_x - 30, pos_y, 15, 15, color(60, 60, 60));
}