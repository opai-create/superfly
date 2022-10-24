#include "config_system.h"
#include "config_vars.h"

#include <ShlObj.h>
#include <fstream>

#include "../includes.h"

#include "../additionals/json/json.h"

typedef Json::Value json;

inline void save_uint(json& j, std::string name, unsigned int v)
{
	j[name] = v;
}

inline void save_float(json& j, std::string name, float v)
{
	j[name] = v;
}

inline void save_int(json& j, std::string name, int v)
{
	j[name] = v;
}

inline void save_bool(json& j, std::string name, bool v)
{
	j[name] = v;
}

inline void save_clr(json& j, std::string name, color* v)
{
	j[name]["h"] = v->get_hue();
	j[name]["s"] = v->get_saturation();
	j[name]["b"] = v->get_brightness();
	j[name]["a"] = v->a();
}

inline void save_bind(json& j, std::string name, const key_bind_info_t& v)
{
	j[name]["key"] = v.key;
	j[name]["type"] = v.type;
	j[name]["toggled"] = v.toggled;
}

inline void load_uint(const json& j, std::string name, unsigned int& v)
{
	if (j[name].empty())
		return;

	v = j[name].asUInt();
}

inline void load_clr(const json& j, std::string name, color& v)
{
	if (j[name].empty())
		return;

	color new_clr = color::hsb(j[name]["h"].asFloat(), j[name]["s"].asFloat(), j[name]["b"].asFloat())
		.manage_alpha(j[name]["a"].asInt());

	v = new_clr;
}

inline void load_bind(json& j, std::string name, key_bind_info_t& v)
{
	if (j[name].empty())
		return;

	v.key = j[name]["key"].asInt();
	v.type = j[name]["type"].asInt();
	v.toggled = j[name]["toggled"].asBool();
}

inline void load_int(const json& j, std::string name, int& v)
{
	if (j[name].empty())
		return;

	v = j[name].asInt();
}

inline void load_bool(const json& j, std::string name, bool& v)
{
	if (j[name].empty())
		return;

	v = j[name].asBool();
}

inline void load_float(const json& j, std::string name, float& v)
{
	if (j[name].empty())
		return;

	v = j[name].asFloat();
}

inline bool is_dir(const TCHAR* dir)
{
	uintptr_t flag = GetFileAttributes(dir);
	if (flag == 0xFFFFFFFFUL)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			return false;
	}
	if (!(flag & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	return true;
}

namespace config
{
	std::string folder = "";

	void create_config_folder()
	{
		log_console("Creating config folder...\n");

		static TCHAR path[MAX_PATH] = {};

		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, path)))
			folder = std::string(path) + "\\superfly\\";

		if (!is_dir(folder.c_str()))
			CreateDirectory(folder.c_str(), NULL);

		log_console(" \n");
	}

	void create(const std::string& config_name)
	{
		std::string file = folder + config_name + ".json";
		std::ofstream file_out(file);
		file_out.close();
	}

	void erase(const std::string& config_name)
	{
		std::string file = folder + config_name + ".json";
		remove(file.c_str());
	}

	void save(const std::string& config_name)
	{
		json json_obj = {};
		
		auto& rage = json_obj["rage"]; {
			save_bool(rage, "enable", g_cfg.rage.enable);

			save_int(rage, "hitchance", g_cfg.rage.hitchance);
			save_int(rage, "mindamage", g_cfg.rage.mindamage);

			save_uint(rage, "auto_", g_cfg.rage.auto_);
		}

		auto& antihit = json_obj["antihit"]; {
			save_bool(antihit, "pitch", g_cfg.antihit.pitch);

			save_bool(antihit, "yaw", g_cfg.antihit.yaw);
			save_bool(antihit, "desync", g_cfg.antihit.desync);
			save_bool(antihit, "invert", g_cfg.antihit.invert);
			save_bool(antihit, "edge", g_cfg.antihit.edge);

			save_bool(antihit, "fakelag", g_cfg.antihit.fakelag);
			save_int(antihit, "fakelag_limit", g_cfg.antihit.fakelag_limit);
		}

		auto& visuals = json_obj["visuals"]; {
			save_bool(visuals, "player", g_cfg.visuals.player);
			save_bool(visuals, "hp", g_cfg.visuals.hp);

			save_bool(visuals, "box", g_cfg.visuals.box);
			save_clr(visuals, "box_clr", &g_cfg.visuals.box_clr);

			save_bool(visuals, "name", g_cfg.visuals.name);
			save_clr(visuals, "name_clr", &g_cfg.visuals.name_clr);

			save_bool(visuals, "weapon", g_cfg.visuals.weapon);
			save_clr(visuals, "weapon_clr", &g_cfg.visuals.weapon_clr);

			save_bool(visuals, "ammo", g_cfg.visuals.ammo);
			save_clr(visuals, "ammo_clr", &g_cfg.visuals.ammo_clr);

			save_bool(visuals, "oof_arrow", g_cfg.visuals.oof_arrow);
			save_clr(visuals, "oof_arrow_clr", &g_cfg.visuals.oof_arrow_clr);

			save_uint(visuals, "flags", g_cfg.visuals.flags);

			for (int i = 0; i < 5; i++) {
				auto sub = "chams " + std::to_string(i + 1);

				save_bool(visuals[sub], "visible", g_cfg.visuals.chams[i].visible);
				save_bool(visuals[sub], "invisible", g_cfg.visuals.chams[i].invisible);

				save_int(visuals[sub], "material", g_cfg.visuals.chams[i].material);

				save_clr(visuals[sub], "visible_clr", &g_cfg.visuals.chams[i].visible_clr);
				save_clr(visuals[sub], "invisible_clr", &g_cfg.visuals.chams[i].invisible_clr);
				
				if (i == local)
					save_bool(visuals[sub], "draw_attachments", g_cfg.visuals.chams[i].draw_attachments);
			}
		}

		auto& misc = json_obj["misc"]; {
			save_bool(misc, "auto_jump", g_cfg.misc.auto_jump);
			save_bool(misc, "auto_strafe", g_cfg.misc.auto_strafe);
			save_bool(misc, "fast_stop", g_cfg.misc.fast_stop);

			for (int i = 0; i < 3; i++) {
				save_int(misc["fov" + std::to_string(i + 1)], "amt_", g_cfg.misc.fovs[i]);
				save_int(misc["viewmodel_pos" + std::to_string(i + 1)], "pos_", g_cfg.misc.viewmodel_pos[i]);
			}

			save_int(misc, "aspect_ratio", g_cfg.misc.aspect_ratio);
			save_int(misc, "thirdperson_dist", g_cfg.misc.thirdperson_dist);

			save_bool(misc, "impacts", g_cfg.misc.impacts);
			save_int(misc, "impact_size", g_cfg.misc.impact_size);

			save_bool(misc, "tracers", g_cfg.misc.tracers);
			save_int(misc, "tracers_size", g_cfg.misc.trace_size);

			for (int i = 0; i < 2; i++)
				save_clr(misc["trace_clr" + std::to_string(i + 1)], "clr_", &g_cfg.misc.trace_clr[i]);

			save_bool(misc, "nightmode", g_cfg.misc.nightmode);
			save_bool(misc, "dynamic_fog", g_cfg.misc.dynamic_fog);

			for (int i = 0; i < 4; i++)
				save_clr(misc["world_clr" + std::to_string(i + 1)], "clr_", &g_cfg.misc.world_clr[i]);

			save_bool(misc, "hitmarker", g_cfg.misc.hitmarker);
			save_bool(misc, "damage", g_cfg.misc.damage);
			save_bool(misc, "sound", g_cfg.misc.sound);
			save_bool(misc, "unlock_inventory", g_cfg.misc.unlock_inventory);
			save_bool(misc, "snip_crosshair", g_cfg.misc.snip_crosshair);

			save_uint(misc, "removals", g_cfg.misc.removals);
		}

		auto& binds = json_obj["binds"]; {
			for (int i = 0; i < binds_max; i++) {
				auto add = std::to_string(i + 1);
				save_bind(binds, add, g_cfg.binds[i]);
			}
		}

		std::string file = folder + config_name + ".json";
		auto str = json_obj.toStyledString();
		std::ofstream file_out(file);
		if (file_out.good())
			file_out << str;
		file_out.close();
	}

	void load(const std::string& config_name)
	{
		std::string file = folder + config_name + ".json";
		std::ifstream input_file = std::ifstream(file);

		if (!input_file.good())
			return;

		json json_obj = {};
		input_file >> json_obj;

		auto& rage = json_obj["rage"]; {
			load_bool(rage, "enable", g_cfg.rage.enable);

			load_int(rage, "hitchance", g_cfg.rage.hitchance);
			load_int(rage, "mindamage", g_cfg.rage.mindamage);

			load_uint(rage, "auto_", g_cfg.rage.auto_);
		}

		auto& antihit = json_obj["antihit"]; {
			load_bool(antihit, "pitch", g_cfg.antihit.pitch);

			load_bool(antihit, "yaw", g_cfg.antihit.yaw);
			load_bool(antihit, "desync", g_cfg.antihit.desync);
			load_bool(antihit, "invert", g_cfg.antihit.invert);
			load_bool(antihit, "edge", g_cfg.antihit.edge);

			load_bool(antihit, "fakelag", g_cfg.antihit.fakelag);
			load_int(antihit, "fakelag_limit", g_cfg.antihit.fakelag_limit);
		}

		auto& visuals = json_obj["visuals"]; {
			load_bool(visuals, "player", g_cfg.visuals.player);
			load_bool(visuals, "hp", g_cfg.visuals.hp);

			load_bool(visuals, "box", g_cfg.visuals.box);
			load_clr(visuals, "box_clr", g_cfg.visuals.box_clr);

			load_bool(visuals, "name", g_cfg.visuals.name);
			load_clr(visuals, "name_clr", g_cfg.visuals.name_clr);

			load_bool(visuals, "weapon", g_cfg.visuals.weapon);
			load_clr(visuals, "weapon_clr", g_cfg.visuals.weapon_clr);

			load_bool(visuals, "ammo", g_cfg.visuals.ammo);
			load_clr(visuals, "ammo_clr", g_cfg.visuals.ammo_clr);

			load_bool(visuals, "oof_arrow", g_cfg.visuals.oof_arrow);
			load_clr(visuals, "oof_arrow_clr", g_cfg.visuals.oof_arrow_clr);

			load_uint(visuals, "flags", g_cfg.visuals.flags);

			for (int i = 0; i < 5; i++) {
				auto sub = "chams " + std::to_string(i + 1);

				load_bool(visuals[sub], "visible", g_cfg.visuals.chams[i].visible);
				load_bool(visuals[sub], "invisible", g_cfg.visuals.chams[i].invisible);

				load_int(visuals[sub], "material", g_cfg.visuals.chams[i].material);

				load_clr(visuals[sub], "visible_clr", g_cfg.visuals.chams[i].visible_clr);
				load_clr(visuals[sub], "invisible_clr", g_cfg.visuals.chams[i].invisible_clr);

				if (i == local)
					load_bool(visuals[sub], "draw_attachments", g_cfg.visuals.chams[i].draw_attachments);
			}
		}

		auto& misc = json_obj["misc"]; {
			load_bool(misc, "auto_jump", g_cfg.misc.auto_jump);
			load_bool(misc, "auto_strafe", g_cfg.misc.auto_strafe);
			load_bool(misc, "fast_stop", g_cfg.misc.fast_stop);

			for (int i = 0; i < 3; i++) {
				load_int(misc["fov" + std::to_string(i + 1)], "amt_", g_cfg.misc.fovs[i]);
				load_int(misc["viewmodel_pos" + std::to_string(i + 1)], "pos_", g_cfg.misc.viewmodel_pos[i]);
			}

			load_int(misc, "aspect_ratio", g_cfg.misc.aspect_ratio);
			load_int(misc, "thirdperson_dist", g_cfg.misc.thirdperson_dist);

			load_bool(misc, "impacts", g_cfg.misc.impacts);
			load_int(misc, "impact_size", g_cfg.misc.impact_size);

			load_bool(misc, "tracers", g_cfg.misc.tracers);
			load_int(misc, "tracers_size", g_cfg.misc.trace_size);

			for (int i = 0; i < 2; i++)
				load_clr(misc["trace_clr" + std::to_string(i + 1)], "clr_", g_cfg.misc.trace_clr[i]);

			load_bool(misc, "nightmode", g_cfg.misc.nightmode);
			load_bool(misc, "dynamic_fog", g_cfg.misc.dynamic_fog);

			for (int i = 0; i < 4; i++)
				load_clr(misc["world_clr" + std::to_string(i + 1)], "clr_", g_cfg.misc.world_clr[i]);

			load_bool(misc, "hitmarker", g_cfg.misc.hitmarker);
			load_bool(misc, "damage", g_cfg.misc.damage);
			load_bool(misc, "sound", g_cfg.misc.sound);
			load_bool(misc, "unlock_inventory", g_cfg.misc.unlock_inventory);
			load_bool(misc, "snip_crosshair", g_cfg.misc.snip_crosshair);

			load_uint(misc, "removals", g_cfg.misc.removals);
		}

		auto& binds = json_obj["binds"]; {
			for (int i = 0; i < binds_max; i++) {
				auto add = std::to_string(i + 1);
				load_bind(binds, add, g_cfg.binds[i]);
			}
		}

		input_file.close();
	}
}