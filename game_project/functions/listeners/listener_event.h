#pragma once
#include "../../base/interfaces/entity_list.h"
#include "../../base/ingame objects/c_csplayer.h"

#include "../../base/interfaces/event_manager.h"

#include <vector>

class c_listener_event : public c_game_event_listener2
{
private:
	std::vector<const char*> events =
	{
		"player_hurt",
		"bullet_impact",
		"round_start",
		"round_end"
	};
public:
	virtual void fire_game_event(c_game_event* event) override;
	virtual int get_event_debug_id() override
	{
		return 42;
	}

	void init_listener();
	void remove_listener();
};

extern c_listener_event* listener_event;