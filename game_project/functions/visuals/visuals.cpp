#include "visuals.h"

#include "esp/nades.h"
#include "esp/player.h"
#include "esp/weapon.h"
#include "esp/event/event_visuals.h"
#include "local/local_player_visuals.h"

#include "../menu/menu.h"

#include "../config_vars.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"

#include "../../base/tools/render.h"

#include "../../base/other/game_functions.h"

#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

#undef local

namespace visuals
{
	void update_buttons()
	{
		for (int i = 0; i < 256; ++i)
		{
			key_t* key = &g::keys[i];

			key->pressed = false;

			if (key->down && key->tick > key->old_tick)
			{
				key->old_tick = key->tick;
				key->pressed = true;
			}
		}
	}

	void proceed_binds()
	{
		for (auto& b : g_cfg.binds)
		{
			if (b.key == -1)
				continue;

			switch (b.type)
			{
			case 0:
				b.toggled = true;
				break;
			case 1:
				b.toggled = g::keys[b.key].down;
				break;
			case 2:
				if (g::keys[b.key].pressed)
					b.toggled = !b.toggled;
				break;
			}
		}
	}

	void on_paint_traverse(c_panel* ptr, unsigned int panel)
	{
		if (std::strcmp(ptr->get_name(panel), "MatSystemTopPanel"))
			return;

		player::render();
		weapon::render();
		nades::render();
		events::hit::render();

		local::draw_pt();

		update_buttons();
		proceed_binds();

		draw_menu();
		//draw_cursor();
	}
}