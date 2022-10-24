#include "chams.h"

#include "../../hvh/anim_correction.h"
#include "../../hvh/lag_compensation.h"

#include <string>

c_material* create_material(const char* material_name, const char* material_data, const char* shader_type = "VertexLitGeneric")
{
	c_key_values* key_values = new c_key_values;
	game_fn::init_key_values(key_values, shader_type, 0, 0);
	game_fn::load_from_buffer(key_values, material_name, material_data, NULL, NULL, NULL, NULL);

	c_material* material = i::material_system->create_material(material_name, key_values);
	material->increment_reference_count();

	return material;
}

c_material* c_chams::get_material(int idx)
{
	switch (idx)
	{
	case 0:
		return soft;
		break;
	case 1:
		return shaded;
		break;
	case 2:
		return flat;
		break;
	case 3:
		return shiny;
		break;
	}
	return nullptr;
}

bool c_chams::draw_chams(const chams_t& chams, matrix3x4_t* matrix)
{
	if (!is_valid_materials())
		return false;

	if (!chams.visible && !chams.invisible)
		return false;

	if (chams.invisible)
	{
		c_material* invisible_material = get_material(chams.material);
		invisible_material->set_material_var_flag(material_var_ignorez, true);
		invisible_material->color_modulate(chams.invisible_clr.r() / 255.f,
			chams.invisible_clr.g() / 255.f,
			chams.invisible_clr.b() / 255.f);
		invisible_material->alpha_modulate(chams.invisible_clr.a() / 255.f);

		this->hook_info.ecx->forced_material_override(invisible_material);
		this->hook_info.call_original(matrix);
	}

	if (chams.visible)
	{
		c_material* visible_material = get_material(chams.material);
		visible_material->set_material_var_flag(material_var_ignorez, false);
		visible_material->color_modulate(chams.visible_clr.r() / 255.f,
			chams.visible_clr.g() / 255.f,
			chams.visible_clr.b() / 255.f);
		visible_material->alpha_modulate(chams.visible_clr.a() / 255.f);

		this->hook_info.ecx->forced_material_override(visible_material);
		this->hook_info.call_original(matrix);
	}
	this->hook_info.ecx->forced_material_override(nullptr);
	return true;
}

void c_chams::on_enemy(c_csplayer* player, bool& render)
{
	if (g::local()->team() == player->team())
		return;

	if (g::local() == player)
		return;

	int index = player->index();
	
	auto old = lag_compensation->get_oldest_record(player);
	if (old)
		draw_chams(g_cfg.visuals.chams[enemy_backtrack], old->bones);

	auto last = lag_compensation->get_latest_record(player);
	if (last) {
		draw_chams(g_cfg.visuals.chams[enemy], last->unresolved_bones);
		draw_chams(g_cfg.visuals.chams[enemy_backtrack], last->left_bones);
		draw_chams(g_cfg.visuals.chams[enemy_shot], last->right_bones);
		render = true;
	}
}

bool c_chams::need_to_watch_behind_local()
{
	c_basecombatweapon* weapon = g::weapon();
	if (!weapon)
		return false;

	// don't need to render chams 
	if (g::local()->is_scoped() || weapon->is_grenade())
		return true;

	return false;
}

void c_chams::on_local_attachments(bool& render)
{
	if (need_to_watch_behind_local())
	{
		this->hook_info.ecx->set_alpha_modulation(0.3f);
		this->hook_info.ecx->forced_material_override(nullptr);

		render = false;
		return;
	}

	if (!g_cfg.visuals.chams[local].draw_attachments)
	{
		render = false;
		return;
	}

	render = draw_chams(g_cfg.visuals.chams[local]);
}

void c_chams::on_local_player(c_csplayer* player, bool& render)
{
	if (g::local()->team() != player->team())
		return;

	if (g::local() != player)
		return;

	draw_chams(g_cfg.visuals.chams[local_fake]);

	if (need_to_watch_behind_local())
	{
		this->hook_info.ecx->set_alpha_modulation(0.3f);
		this->hook_info.ecx->forced_material_override(nullptr);

		render = false;
		return;
	}

	render = draw_chams(g_cfg.visuals.chams[local]);
}

bool c_chams::should_render()
{
	if (!g::in_game)
		return false;

	bool should_render = false;

	c_client_entity* client_entity = (c_client_entity*)this->hook_info.info.client_entity;
	if (!client_entity)
		return false;

	c_baseentity* entity = client_entity->get_i_unknown_entity()->get_base_entity();
	if (!entity)
		return false;

	if (entity->is_player())
	{
		c_csplayer* player = (c_csplayer*)entity;

		if (!player || !player->is_alive())
			return false;

		if (player->dormant())
			return false;

		this->on_local_player(player, should_render);
		this->on_enemy(player, should_render);
	}

	// attachments
	c_baseentity* move_parent = entity->get_move_parent();
	if (move_parent && move_parent == g::local())
		this->on_local_attachments(should_render);

	return should_render;
}

void c_chams::create_materials()
{
	static bool once = false;
	if (once)
		return;

	soft = create_material("csgo_soft", R"#("VertexLitGeneric"
			{
				"$ambientonly"					"1"
			}
		)#");

	shaded = create_material("csgo_shaded", R"#("VertexLitGeneric"
			{
				"$basetexture"				    "vgui/white"
				"$phong"						"1"
				"$phongexponent"				"0"
				"$phongboost"					"0"
				"$rimlight"					    "1"
				"$rimlightexponent"		        "0"
				"$rimlightboost"		        "0"
				"$model"						"1"
				"$nocull"						"0"
				"$halflambert"				    "0"
				"$basemapalphaphongmask"		"1"
			}
		)#");

	flat = create_material("csgo_flat", R"#("UnlitGeneric"
			{
				"$flat"							"1"
				"%noToolTexture"				"1"
			}
		)#", "UnlitGeneric");

	shiny = create_material("csgo_shiny", R"#("VertexLitGeneric"
			{
				"$basetexture"						"vgui/white_additive"
				"$envmap"							"env_cubemap"
				"$normalmapalphaenvmapmask"			"1"
				"$basemapalphaphongmask"			"1"
				"$envmapcontrast"					"0"
				"$envmaptint"						"[0.3 0.3 0.3]"
				"$nofog"							"1"
				"$model"							"1"
				"$nocull"							"0"
				"$selfillum"						"1"
				"$halflambert"						"1"
				"$znearer"							"0"
			}
		)#", "VertexLitGeneric");

	if (is_valid_materials())
		once = true;
}

void c_chams::init(draw_model_fn original, c_studio_render* ecx, void* results, const draw_model_info_t& info,
	matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vector3d& model_origin, int flags)
{
	this->create_materials();

	this->hook_info.init(original, ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, model_origin, flags);
}