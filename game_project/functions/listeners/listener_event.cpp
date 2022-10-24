#include "listener_event.h"

#include "../hvh/resolver.h"
#include "../aimbot/ragebot.h"

#include "../visuals/esp/event/event_visuals.h"

void c_listener_event::fire_game_event(c_game_event* event)
{
	visuals::events::hit::parse(event);
	resolver::on_round_start(event);
	//aim_utils::on_round_events(event);
}

void c_listener_event::init_listener()
{
	for (const auto& event : this->events)
		i::game_event_manager->add_listener(this, event, false);
}

void c_listener_event::remove_listener()
{
	i::game_event_manager->remove_listener(this);

	this->events.clear();
}

c_listener_event* listener_event = new c_listener_event();