#pragma once
#include "../base/other/color.h"
#include "menu/menu.h"

#include <array>

enum chams_type
{
	enemy = 0,
	enemy_backtrack,
	enemy_shot,
	local,
	local_fake,
};

enum fov_t
{
	world = 0,
	arms,
	zoom
};

enum world_clr_t
{
	walls = 0,
	props,
	sky,
	fog
};

enum world_esp
{
	wpn = 0,
	nade
};

enum removals_t
{
	scope = 1,
	vis_recoil = 2,
	post_process = 4,
	smoke = 8,
	flash = 16,
	fog_ = 32,
	viewm_bob = 64,
	land_bob = 128
};

enum binds_idx_t
{
	fd_b = 0,
	sw_b,
	dt_b,
	hs_b,
	ap_b,
	tp_b,
	inv_b,
	binds_max
};

struct chams_t
{
	bool visible = false;
	bool invisible = false;

	int material = 0;

	color visible_clr = color();
	color invisible_clr = color();

	bool draw_attachments = false;

	chams_t() { };

	chams_t(bool visible, bool invisible, int material,
		const color& visible_clr, const color& invisible_clr, bool draw_attachments) :

		visible(visible), invisible(invisible), material(material),
		visible_clr(visible_clr), invisible_clr(invisible_clr), draw_attachments(draw_attachments)
	{ }
};

struct key_bind_info_t;

struct config_t
{
	struct rage_t
	{
		bool enable = false;

		int hitchance = 0;
		int mindamage = 0;

		unsigned int auto_ = 0;
	}rage;

	struct antihit_t
	{
		bool pitch = false;

		bool yaw = false;
		bool desync = false;
		bool invert = false;
		bool edge = false;

		bool fakelag = false;
		int fakelag_limit = 14;
	}antihit;

	struct visuals_t
	{
		bool player = false;

		bool box = false;
		color box_clr = color(255, 255, 255, 240);

		bool name = false;
		color name_clr = color(255, 255, 255, 240);

		bool hp = false;

		bool weapon = false;
		color weapon_clr = color(255, 255, 255, 240);

		bool ammo = false;
		color ammo_clr = color(250, 70, 70, 240);

		bool oof_arrow = false;
		color oof_arrow_clr = color(250, 70, 70, 255);

		unsigned int flags = 0;

		chams_t chams[5] =
		{
			chams_t(false, false, 0, color(157, 157, 224), color(120, 255, 228), false),
			chams_t(false, false, 1, color(), color(), false),
			chams_t(false, false, 0, color(), color(), false),
			chams_t(false, false, 0, color(), color(), false),
			chams_t(false, false, 1, color(), color(), false)
		};

		struct world_t
		{
			bool enable = false;

			bool box = false;
			color box_clr = color(255, 255, 255, 240);

			bool name = false;
			color name_clr = color(255, 255, 255, 240);

			bool ammo = false;
			color ammo_clr = color(250, 70, 70, 240);

			unsigned int timer = 0;
		}world[2];
	}visuals;

	struct misc_t
	{
		bool menu = true;

		bool auto_jump = false;
		bool auto_strafe = false;

		bool fast_stop = false;

		int fovs[3] = {};
		int viewmodel_pos[3] = {};

		int aspect_ratio = 0;

		int thirdperson_dist = 100;

		bool impacts = false;
		int impact_size = 15;

		bool tracers = false;
		int trace_size = 10;

		color trace_clr[2] =
		{
			color(255, 0, 0),
			color(127, 250, 225)
		};

		bool nightmode = false;
		bool dynamic_fog = false;

		color world_clr[4]
		{
			color(16, 16, 22),
			color(200, 104, 109, 255),
			color(78, 153, 138),
			color(149, 149, 180)
		};

		bool hitmarker = false;
		bool damage = false;
		bool sound = false;

		bool unlock_inventory = false;
		bool snip_crosshair = false;

		unsigned int removals = 0;
	}misc;

	std::array<key_bind_info_t, binds_max> binds;
};

extern config_t g_cfg;