#include "menu.h"

#include "../config_system.h"
#include "../config_vars.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"
#include "../../base/tools/render.h"

#include <ShlObj.h>
#include <algorithm>

menu_info_t menu_info = {};

const int array_cnt = 5;

std::array<std::string, array_cnt> tabs =
{
	"ragebot",
	"anti-hit",
	"visuals",
	"extra",
	"configs"
};

int cursor_pos[2] = {};

bool item_hovered(int x, int y, int w, int h)
{
	return cursor_pos[0] > x && cursor_pos[1] > y
		&& cursor_pos[0] < x + w && cursor_pos[1] < y + h;
}

int selector = 0;
int chams_type[2] = {};

void ragebot_tab()
{
	checkbox("enable rage", &g_cfg.rage.enable, 0);

	slider("hitchance", &g_cfg.rage.hitchance, 0, 100, 1, "%");
	slider("mindamage", &g_cfg.rage.mindamage, 0, 120, 2, "hp");
	
	multi_combo_box("auto", &g_cfg.rage.auto_, 3, { "fire", "stop", "scope", "revolver" });
}

void anti_hit_tab()
{
	checkbox("pitch anti aim", &g_cfg.antihit.pitch, 0);

	checkbox("yaw anti aim", &g_cfg.antihit.yaw, 2);
	checkbox("fake angle", &g_cfg.antihit.desync, 3);
	key_bind("inverter", g_cfg.binds[inv_b], 4);
	checkbox("jitter mode", &g_cfg.antihit.invert, 5);
	checkbox("wall anti-aim", &g_cfg.antihit.edge, 6);

	checkbox("fake lag", &g_cfg.antihit.fakelag, 7);
	slider("lag limit", &g_cfg.antihit.fakelag_limit, 1, 14, 8, "ticks");

	key_bind("fake duck", g_cfg.binds[fd_b], 9);
	key_bind("slow walk", g_cfg.binds[sw_b], 10);

	key_bind("hide shots", g_cfg.binds[hs_b], 11);
	key_bind("double tap", g_cfg.binds[dt_b], 12);
}

void visuals_tab()
{
	static int sub_tab = 1;
	button("<", 0, 0, []()
		{
			sub_tab--;
		});

	button(">", 0, 1, []()
		{
			sub_tab++;
		});

	sub_tab = std::clamp(sub_tab, 1, 5);

	std::string sub_tab_str = std::to_string(sub_tab) + " / 5";

	render::text(menu_info.pos_x + menu_info.size_x / 2, menu_info.pos_y + 15,
		color(), render::fonts::menu, centered_x, sub_tab_str);

	switch (sub_tab)
	{
	case 1:
	{
		// enemy esp
		checkbox("enable esp", &g_cfg.visuals.player, 1);

		checkbox("box", &g_cfg.visuals.box, 2);
		color_picker("box color", &g_cfg.visuals.box_clr, 3, false);

		checkbox("name", &g_cfg.visuals.name, 4);
		color_picker("name color", &g_cfg.visuals.name_clr, 5, false);

		checkbox("hp", &g_cfg.visuals.hp, 6);

		checkbox("weapon", &g_cfg.visuals.weapon, 7);
		color_picker("weapon color", &g_cfg.visuals.weapon_clr, 8, false);

		checkbox("ammo", &g_cfg.visuals.ammo, 9);
		color_picker("ammo color", &g_cfg.visuals.ammo_clr, 10, false);

		checkbox("out of fov indicator", &g_cfg.visuals.oof_arrow, 11);
		color_picker("out of fov color", &g_cfg.visuals.oof_arrow_clr, 12, false);

		multi_combo_box("flags", &g_cfg.visuals.flags, 13,
			{ "money", "armor", "scope", "flash", "defuse kit", "defusing", "bomb" });

	}
	break;
	case 2:
	{
		// enemy chams (default, backtrack, shot)
		bool draw_combo = combo_box("enemy chams type", &chams_type[0], 1, { "player", "backtrack", "shot record" });

		if (chams_type[0] != -1)
		{
			checkbox("visible", &g_cfg.visuals.chams[chams_type[0]].visible, 2, !draw_combo);
			checkbox("through walls", &g_cfg.visuals.chams[chams_type[0]].invisible, 3, !draw_combo);

			bool draw_combo_mat = combo_box("material", &g_cfg.visuals.chams[chams_type[0]].material,
				4, { "soft", "shaded", "flat", "shiny" }, !draw_combo);

			color_picker("visible model color", &g_cfg.visuals.chams[chams_type[0]].visible_clr, 5, true, !draw_combo_mat);
			color_picker("through walls model color", &g_cfg.visuals.chams[chams_type[0]].invisible_clr, 6, true, !draw_combo_mat);
		}
	}
	break;
	case 3:
	{
		// local chams (default, fake)
		bool draw_combo = combo_box("local chams type", &chams_type[1], 1, { "player", "fake" });

		if (chams_type[1] != -1)
		{
			checkbox("visible", &g_cfg.visuals.chams[3 + chams_type[1]].visible, 2, !draw_combo);
			checkbox("through walls", &g_cfg.visuals.chams[3 + chams_type[1]].invisible, 3, !draw_combo);

			bool draw_combo_mat = combo_box("material", &g_cfg.visuals.chams[3 + chams_type[1]].material,
				4, { "soft", "shaded", "flat", "shiny" }, !draw_combo);

			color_picker("visible model color", &g_cfg.visuals.chams[3 + chams_type[1]].visible_clr, 5, true, !draw_combo_mat);
			color_picker("through walls model color", &g_cfg.visuals.chams[3 + chams_type[1]].invisible_clr, 6, true, !draw_combo_mat);

			if (chams_type[1] == 0)
				checkbox("apply on attachments", &g_cfg.visuals.chams[3].draw_attachments, 7, !draw_combo_mat);
		}
	}
	break;
	case 4:
	{
		// world esp (weapon)
		checkbox("enable weapon esp", &g_cfg.visuals.world[0].enable, 1);

		checkbox("weapon box", &g_cfg.visuals.world[wpn].box, 2);
		color_picker("weapon box color", &g_cfg.visuals.world[wpn].box_clr, 3, false);

		checkbox("weapon name", &g_cfg.visuals.world[wpn].name, 4);
		color_picker("weapon name color", &g_cfg.visuals.world[wpn].name_clr, 5, false);

		checkbox("weapon ammo", &g_cfg.visuals.world[wpn].ammo, 6);
		color_picker("weapon ammo color", &g_cfg.visuals.world[wpn].ammo_clr, 7, false);

		//checkbox("bomb timer", &config::esp::world_weapon::bomb_timer, 8);
	}
	break;
	case 5:
	{
		// world esp (grenades)
		checkbox("enable nade esp", &g_cfg.visuals.world[nade].enable, 1);

		checkbox("nade box", &g_cfg.visuals.world[nade].box, 2);
		color_picker("nade box color", &g_cfg.visuals.world[nade].box_clr, 3, false);

		checkbox("nade name", &g_cfg.visuals.world[nade].name, 4);
		color_picker("nade name color", &g_cfg.visuals.world[nade].name_clr, 5, false);

		multi_combo_box("nade timer", &g_cfg.visuals.world[nade].timer, 6, { "molotov", "smoke" });
	}
	break;
	}
}

void misc_tab()
{
	static int sub_tab = 1;
	button("<", 0, 0, []()
		{
			sub_tab--;
		});

	button(">", 0, 1, []()
		{
			sub_tab++;
		});

	sub_tab = std::clamp(sub_tab, 1, 5);

	std::string sub_tab_str = std::to_string(sub_tab) + " / 5";

	render::text(menu_info.pos_x + menu_info.size_x / 2, menu_info.pos_y + 15,
		color(), render::fonts::menu, centered_x, sub_tab_str);

	switch (sub_tab)
	{
	case 1:
	{
		// movement tab
		checkbox("auto jump", &g_cfg.misc.auto_jump, 1);
		checkbox("auto strafe", &g_cfg.misc.auto_strafe, 2);
		checkbox("fast stop", &g_cfg.misc.fast_stop, 3);
		key_bind("auto peek", g_cfg.binds[ap_b], 4);
	}
	break;
	case 2:
	{
		slider("add world fov", &g_cfg.misc.fovs[world], 0, 30, 1, "degrees");
		slider("add viewmodel fov", &g_cfg.misc.fovs[arms], 0, 30, 2, "degrees");
		slider("zoom fov", &g_cfg.misc.fovs[zoom], 0, 100, 3, "percent");

		slider("view offset x", &g_cfg.misc.viewmodel_pos[0], -20, 20, 6, "pixels");
		slider("view offset y", &g_cfg.misc.viewmodel_pos[1], -20, 20, 7, "pixels");
		slider("view offset z", &g_cfg.misc.viewmodel_pos[2], -20, 20, 8, "pixels");

		slider("aspect ratio", &g_cfg.misc.aspect_ratio, 0, 100, 10, "%");

		checkbox("sniper crosshair", &g_cfg.misc.snip_crosshair, 12);
	}
	break;
	case 3:
	{
		// world tab
		checkbox("night mode", &g_cfg.misc.nightmode, 1);

		color_picker("world color", &g_cfg.misc.world_clr[world], 2, false);
		color_picker("props color", &g_cfg.misc.world_clr[props], 3, true);
		color_picker("skybox color", &g_cfg.misc.world_clr[sky], 4, false);
		color_picker("fog color", &g_cfg.misc.world_clr[fog], 5, false);

		checkbox("dynamic fog radius", &g_cfg.misc.dynamic_fog, 6);
	}
	break;
	case 4:
	{
		// hit marker tab
		checkbox("hit marker", &g_cfg.misc.hitmarker, 1);
		checkbox("hit shound", &g_cfg.misc.sound, 2);
		checkbox("show damage", &g_cfg.misc.damage, 3);

		checkbox("impacts", &g_cfg.misc.impacts, 4);
		slider("size", &g_cfg.misc.impact_size, 10, 25, 5, "");

		checkbox("bullet tracers", &g_cfg.misc.tracers, 6);
		slider("size", &g_cfg.misc.trace_size, 10, 25, 7, "");
		color_picker("bullet tracers enemy color", &g_cfg.misc.trace_clr[0], 8, false);
		color_picker("bullet tracers local color", &g_cfg.misc.trace_clr[1], 9, false);
	}
	break;
	case 5:
	{
		// misc tab
		bool draw_removals = multi_combo_box("removals", &g_cfg.misc.removals, 1,
			{ "scope overlay", "visual recoil", "post processing", "smoke effect", "flashbang effect", "fog effect", "viewmodel bob", "landing bob" });

		checkbox("unlock inventory", &g_cfg.misc.unlock_inventory, 2, !draw_removals);

		key_bind("third person", g_cfg.binds[tp_b], 3, !draw_removals);
		slider("third person distance", &g_cfg.misc.thirdperson_dist, 50, 300, 4, "ft", !draw_removals);

		button("hidden convars", 5, 2, []()
		{
			if (!i::convar)
				return;

			auto it = *(c_cvar**)(i::convar + 0x34);
			for (auto cv = it->next; cv != nullptr; cv = cv->next)
			{
				cv->flags &= ~(1 << 1);
				cv->flags &= ~(1 << 4);
			}
		}, !draw_removals);
	}
	break;
	}
}

int cfg_index = -1;

std::vector<std::string> config_names = { "main", "additional", "misc", "custom 1", "custom 2" };

void configs_tab()
{
	bool draw_combo = combo_box("config slot", &cfg_index, 0, { "main", "additional", "misc", "custom 1", "custom 2" });
	button("save", 1, 0, []()
		{
			if (cfg_index != -1)
				config::save(config_names[cfg_index]);
		});

	button("load", 1, 1, []()
		{
			if (cfg_index != -1)
				config::load(config_names[cfg_index]);
		}, !draw_combo);

	button("uninject", 2, 2, []()
		{
			g::uninject = true;
		}, !draw_combo);
}

void draw_tabs()
{
	switch (selector)
	{
	case 0:
		ragebot_tab();
		break;
	case 1:
		anti_hit_tab();
		break;
	case 2:
		visuals_tab();
		break;
	case 3:
		misc_tab();
		break;
	case 4:
		configs_tab();
		break;
	}
}

void draw_cursor()
{
	const int size = 15;
	int x = cursor_pos[0];
	int y = cursor_pos[1];

	if (g_cfg.misc.menu)
	{
		render::triangle_filled(vector2d(x, y),
			vector2d(x + size + 1, y + size),
			vector2d(x, y + size),
			color(255, 133, 139));

		render::triangle_filled(vector2d(x, y + size),
			vector2d(x + size / 2, y + size),
			vector2d(x, y + size + 8),
			color(255, 133, 139));

		SetCursor(NULL);
	}
}

void draw_menu()
{
	if (!g_cfg.misc.menu)
		return;

	i::surface->surface_get_cursor_pos(cursor_pos[0], cursor_pos[1]);

	int center_x = render::screen.w / 2;
	int center_y = render::screen.h / 2;

	// window header
	render::rect_filled(center_x - 168, center_y - 165, 337, 20, color(46, 46, 46));
	render::rect(center_x - 168, center_y - 165, 337, 20, color(66, 66, 66));
	render::text(center_x, center_y - 163, color(), render::fonts::menu, centered_x, "superfly");

	// main window
	render::rect_filled(center_x - 168, center_y - 145, 337, 330, color(36, 36, 36));
	render::rect(center_x - 168, center_y - 145, 337, 330, color(46, 46, 46));

	// tabs
	menu_info.write_size(337, 330);
	menu_info.write_pos(center_x - 168, center_y - 125);

	for (int i = 0; i < array_cnt; i++)
	{
		auto w = render::text_width(render::fonts::menu, tabs[i].c_str());

		int tab_size = 336 / array_cnt;
		int button_pos = (center_x - 168) + tab_size * i;

		bool hovered = item_hovered(button_pos, center_y - 144, tab_size, 20);

		if (hovered && g::keys[VK_LBUTTON].pressed)
			selector = i;

		const color clr = hovered ? color(60, 60, 60) : color(50, 50, 50);
		const color text_clr = selector == i ? color(255, 133, 139) : color();

		render::rect_filled(button_pos + 1, center_y - 144, tab_size, 20, clr);
		render::rect(button_pos + 1, center_y - 144, tab_size, 20, color(70, 70, 70));
		render::text(button_pos + tab_size / 2, center_y - 141, text_clr, render::fonts::menu, centered_x, tabs[i]);
	}

	draw_tabs();
}