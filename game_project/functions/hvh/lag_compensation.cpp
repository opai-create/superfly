#include "lag_compensation.h"

#include "resolver.h"
#include "anim_correction.h"

#include "../listeners/listener_entity.h"
#include "../../additionals/threading/threading.h"

// rifk animfix have good implenetation 
// but it should be reworked alot

/* TO-DO:
	 -fixes when enemy go out from dormant
	(done) -break lc (lag peekers) fixes
	(done) -safe-point matrix
*/

float c_lag_compensation::get_lerp_time()
{
	static auto cl_updaterate = i::convar->find_convar("cl_updaterate");
	static auto cl_interp = i::convar->find_convar("cl_interp");

	const auto update_rate = cl_updaterate->get_int();
	const auto interp_ratio = cl_interp->get_float();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= interp_ratio)
		lerp = interp_ratio;

	return lerp;
}

bool record_t::is_valid(float range = .2f, float max_unlag = .2f)
{
	if (!i::engine->get_net_channel_info() || !valid)
		return false;

	const auto correct = std::clamp(i::engine->get_net_channel_info()->get_latency(flow_incoming)
		+ i::engine->get_net_channel_info()->get_latency(flow_outgoing)
		+ lag_compensation->get_lerp_time(), 0.f, max_unlag);

	float curtime = g::local()->is_alive() ?
		math::ticks_to_time(g::tick_base) : i::global_vars->cur_time;

	return std::fabsf(correct - (curtime - sim_time)) <= range;
}

void record_t::parse(c_csplayer* player)
{
	const auto weapon = player->get_active_weapon();
	this->player = player;

	index = this->player->index();
	dormant = this->player->dormant();
	velocity = this->player->velocity();
	origin = this->player->origin();
	abs_origin = this->player->get_abs_origin();
	obb_mins = this->player->mins();
	obb_maxs = this->player->maxs();

	this->player->parse_layer(layers[original_side]);
	this->player->parse_pose(poses);

	anim_state = this->player->get_animation_state();
	sim_time = this->player->simulation_time();
	old_simtime = this->player->old_simulation_time();
	last_shot_time = weapon ? weapon->last_shot_time() : 0.f;
	duck = this->player->duck_amount();
	lby = this->player->lby();
	thirdperson_recoil = player->thirdperson_recoil();
	eye_angles = this->player->eye_angles();
	abs_ang = this->player->get_abs_angles();
	flags = this->player->flags();
	eflags = this->player->e_flags();
	effects = this->player->effects();

	anim_speed = 0.f;
	land_time = 0.f;
	time_delta = std::max(i::global_vars->interval_per_tick, sim_time - old_simtime);

	lag = std::clamp(math::time_to_ticks(time_delta), 0, 17);
	didshot = false;
	valid = true;
}

record_t::record_t(c_csplayer* player)
{
	this->parse(player);
}

void record_t::restore(c_csplayer* player) const
{
	player->velocity() = velocity;
	player->flags() = flags;
	player->e_flags() = eflags;
	player->duck_amount() = duck;
	std::memcpy(player->get_animation_layer(), layers[original_side], sizeof(c_animationlayer) * 13);
	player->lby() = lby;
	player->thirdperson_recoil() = thirdperson_recoil;
	player->origin() = origin;
	game_fn::set_abs_origin(player, origin);
}

void record_t::apply(c_csplayer* player) const
{
	std::memcpy(player->get_animation_layer(), layers[original_side], sizeof(c_animationlayer) * 13);
	player->velocity() = velocity;
	player->origin() = origin;
	game_fn::set_abs_origin(player, origin);
	player->flags() = flags;
	player->eye_angles() = eye_angles;
	player->duck_amount() = duck;
}

void invalidate_bones(c_csplayer* player, matrix3x4_t* matrix)
{
	int size_of_bones = sizeof(matrix3x4_t) * player->get_bone_cache().count();

	std::memcpy(player->get_bone_cache().base(), matrix, size_of_bones);

	if (player->get_bone_cache().base() != player->get_bone_accessor()->bones)
		std::memcpy(player->get_bone_accessor()->bones, player->get_bone_cache().base(), size_of_bones);
}

void setup_thread_bones(c_setupbones* bones)
{
	g::setup_bones = true; 
	bones->setup();
	g::setup_bones = false;

	/*bones->fix_bones_rotations();
	invalidate_bones(bones->animating, bones->bones);
	bones->attachment_helper();*/
}

void record_t::setup_bones(c_csplayer* player, int idx)
{
	switch (idx)
	{
	case 0:
		bone_setup[0].prepare_data(player, bones, setup_mask);
		break;
	case 1:
		bone_setup[1].prepare_data(player, unresolved_bones, setup_mask);
		break;
	case 2:
		bone_setup[2].prepare_data(player, left_bones, setup_mask);
		break;
	case 3:
		bone_setup[3].prepare_data(player, right_bones, setup_mask);
		break;
	}
	Threading::QueueJob(setup_thread_bones, bone_setup[idx]);
}

void c_lag_compensation::store_records(int stage)
{
	if (!g::in_game || g::uninject)
	{
		// don't need to setup bones on this moment
		Threading::FinishQueue();
		return;
	}

	if (stage != frame_net_update_end)
		return;

	for (auto it = animation_infos.begin(); it != animation_infos.end();)
	{
		c_csplayer* player = (c_csplayer*)i::entity_list->get_entity_handle(it->first);

		if (!it->second)
			continue;

		if (!player || player != it->second->player || !player->is_alive() || !g::local()) 
		{
			if (player)
				player->clientside_anim() = true;
			it = animation_infos.erase(it);
		}
		else
			it = std::next(it);
	}

	auto player_array = listener_entity->get_entity(ent_player);
	if (player_array.empty())
		return;

	for (auto player_info : player_array)
	{
		auto player = (c_csplayer*)player_info.entity;
		if (!player)
			continue;

		if (!player->is_alive())
		{
			resolver::info[player->index() - 1].reset();
			continue;
		}

		if (player == g::local())
			continue;

		// don't need for full animfix
		// but we should still correct some animations
		if (player->have_immunity() || player->team() == g::local()->team())
		{
			anim_correction::force_update(player, player->simulation_time() == player->old_simulation_time());
			continue;
		}

		if (player->dormant())
			continue;

		if (animation_infos.find(player->get_ref_handle()) == animation_infos.end())
			animation_infos.insert_or_assign(player->get_ref_handle(), new animation_info(player, {}));
	}

	// run post update
	for (auto& info : animation_infos)
	{
		auto& _animation = info.second;
		auto player = _animation->player;

		// don't animate invalid players / break lc abusers
		if (player->simulation_time() <= player->old_simulation_time())
			continue;

		c_basecombatweapon* weapon = player->get_active_weapon();
		if (!weapon)
			continue;

		c_csplayer* world_weapon = (c_csplayer*)weapon->get_weapon_world_model();
		if (!world_weapon)
			continue;

		// update and invalidate bone merge cache when needed
		// TO-DO: fix crash and rework realloc method
		//auto& bone_merge = bone_merge::get_bone_merge_ptr(world_weapon);

		//if (!bone_merge)
		//{
		//	// 676 - sizeof CBoneMergeCache class
		//	bone_merge = (uintptr_t)interfaces::memory->realloc((void*)bone_merge, 676);

		//	game_fn::bone_merge_invalidate(bone_merge);
		//	game_fn::bone_merge_init(bone_merge, world_weapon);
		//}
		//else
		//	bone_merge::update_cache(bone_merge);

		// reset animstate
		if (_animation->last_spawn_time != player->spawn_time())
		{
			const auto state = player->get_animation_state();
			if (state)
			{
				state->player = player;
				state->reset();
			}

			_animation->last_spawn_time = player->spawn_time();
		}

		// grab previous and pre previous ticks
		if (!_animation->frames.empty() && !_animation->frames.front().dormant
			&& math::time_to_ticks(player->simulation_time() - _animation->frames.front().sim_time) <= 17)
		{
			_animation->last = &_animation->frames.front();
			if (_animation->frames.size() >= 3)
				_animation->previous = &_animation->frames[2];

			resolver::init(player, _animation->last, resolver::info[player->index() - 1]);
		}

		_animation->current = &_animation->frames.emplace_front(player);
		_animation->update();

		// too much records, needs to erase
		while (_animation->frames.size() > g::tick_rate)
			_animation->frames.pop_back();
	}

	Threading::FinishQueue(true);
}

record_t* c_lag_compensation::get_latest_record(c_csplayer* player)
{
	const auto info = animation_infos.find(player->get_ref_handle());

	if (info == animation_infos.end() || info->second->frames.empty())
		return nullptr;

	for (auto it = info->second->frames.begin(); it != info->second->frames.end(); it = next(it))
		if ((it)->is_valid())
			return &*it;

	return nullptr;
}

record_t* c_lag_compensation::get_oldest_record(c_csplayer* player)
{
	const auto info = animation_infos.find(player->get_ref_handle());

	if (info == animation_infos.end() || info->second->frames.empty())
		return nullptr;

	for (auto it = info->second->frames.rbegin(); it != info->second->frames.rend(); it = next(it))
		if ((it)->is_valid())
			return &*it;

	return nullptr;
}

void c_lag_compensation::animation_info::update_animations(int idx)
{
	auto state = player->get_animation_state();
	if (!state)
		return;

	if (last && !last->dormant)
	{
		state->primary_cycle = last->layers[original_side][6].cycle;
		state->move_weight = last->layers[original_side][6].weight;
	
		state->strafe_sequence = last->layers[original_side][7].sequence;
		state->strafe_change_weight = last->layers[original_side][7].weight;
		state->strafe_change_cycle = last->layers[original_side][7].cycle;
		state->acceleration_weight = last->layers[original_side][12].weight;
	
		player->set_layer(last->layers[original_side]);
	}
	else
	{
		// this happenes only when first record arrived
		// so we should set latest data as soon as possible
		if (current->flags & fl_onground)
		{
			state->on_ground = true;
			state->landing = false;
		}
	
		state->primary_cycle = current->layers[original_side][6].cycle;
		state->move_weight = current->layers[original_side][6].weight;
	
		state->strafe_sequence = current->layers[original_side][7].sequence;
		state->strafe_change_weight = current->layers[original_side][7].weight;
		state->strafe_change_cycle = current->layers[original_side][7].cycle;
		state->acceleration_weight = current->layers[original_side][12].weight;
	
		state->duration_in_air = 0.f;
		player->pose_parameter()[6] = 0.f;
	
		player->set_layer(current->layers[original_side]);
		state->last_update_time = (current->sim_time - i::global_vars->interval_per_tick);
	}

	if (!last || current->lag <= 1)
	{
		current->apply(player);

		player->lby() = current->lby;
		player->thirdperson_recoil() = current->thirdperson_recoil;

		if (idx != 2)
			state->abs_yaw = math::normalize(player->eye_angles().y + (state->get_max_body_rotation() * idx));
		else
			resolver::start(player);

		anim_correction::force_update(player);
	}
	else
	{
		player->lby() = last->lby;
		player->thirdperson_recoil() = last->thirdperson_recoil;

		for (int i = 0; i < current->lag; i++)
		{
			float new_simtime = current->old_simtime + math::ticks_to_time(i + 1);
			float lerp = 1.f - (current->sim_time - new_simtime) / (current->sim_time - current->old_simtime);

			game_fn::set_abs_origin(player, current->origin);
			player->flags() = current->flags;

			if (current->on_ground)
			{
				player->flags() |= fl_onground;

				auto layer = &player->get_animation_layer()[4];
				layer->cycle = layer->weight = 0.f;
			}
			else
				player->flags() &= ~fl_onground;

			player->eye_angles() = math::normalize(math::interpolate(last->eye_angles, current->eye_angles, lerp));
			player->duck_amount() = math::interpolate(last->duck, current->duck, lerp);

			if (idx != 2)
				state->abs_yaw = math::normalize(player->eye_angles().y + (state->get_max_body_rotation() * idx));
			else
				resolver::start(player);

			if (current->didshot)
			{
				if (current->last_shot_time <= new_simtime)
				{
					player->lby() = current->lby;
					player->thirdperson_recoil() = current->thirdperson_recoil;
				}
			}

			player->velocity() = current->velocity;

			float old_simtime = player->simulation_time();
			player->simulation_time() = new_simtime;
			anim_correction::force_update(player);
			player->simulation_time() = old_simtime;
		}
	}

	if (idx == 2)
	{
		player->set_layer(current->layers[original_side]);
		current->setup_bones(player, 0);
	}
	else
	{
		switch (idx)
		{
		case -1:
			player->parse_layer(current->layers[left_side]);
			current->setup_bones(player, 2);
			break;
		case 1:
			player->parse_layer(current->layers[right_side]);
			current->setup_bones(player, 3);
			break;
		case 0:
			player->parse_layer(current->layers[zero_side]);
			current->setup_bones(player, 1);
			break;
		}
		player->set_layer(current->layers[original_side]);
	}
}

void c_lag_compensation::animation_info::update()
{
	auto old_record = new record_t(player);

	int idx = player->index() - 1;
	auto& info = resolver::info[idx];
	std::memcpy(info.pre_orig, old_record->layers[original_side], sizeof(c_animationlayer) * 13);
	info.got_layer = true;

	auto old_state = *player->get_animation_state();

	current->on_ground = current->flags & fl_onground;

	if (last && !last->dormant)
	{
		auto weapon = player->get_active_weapon();
		if (weapon)
		{
			auto world_weapon = (c_csplayer*)weapon->get_weapon_world_model();
			if (world_weapon)
			{
				for (int i = 0; i < 13; i++)
				{
					auto layer = &player->get_animation_layer()[i];
					layer->owner = player;
					layer->studio_hdr = player->get_studio_hdr();

					if (layer->sequence >= 2 && layer->weight > 0.f)
						player->update_dispatch_layer(layer, world_weapon->get_studio_hdr(), layer->sequence);
				}
			}

			current->last_shot_time = weapon->last_shot_time();
			current->didshot = current->sim_time >= current->last_shot_time && current->last_shot_time > current->old_simtime;
		}

		current->was_in_air = current->player->flags() & fl_onground && last->flags & fl_onground;
		current->origin_diff = current->origin - last->origin;

		vector3d new_velocity = current->player->velocity();

		if (current->origin_diff.valid() && math::time_to_ticks(current->time_delta) > 0) 
		{
			new_velocity = current->origin_diff * (1.f / current->time_delta);

			if (current->player->flags() & fl_onground
				&& current->layers[original_side][11].weight > 0.f && current->layers[original_side][11].weight < 1.f
				&& current->layers[original_side][11].cycle > last->layers[original_side][11].cycle)
			{
				if (weapon)
				{
					float max_speed = 260.f;
					auto weapon_info = weapon->get_weapon_info();

					if (weapon_info)
						max_speed = current->player->is_scoped() ? weapon_info->max_speed_alt : weapon_info->max_speed;

					float modifier = 0.35f * (1.f - current->layers[original_side][11].weight);

					if (modifier > 0.f && modifier < 1.f)
						current->anim_speed = max_speed * (modifier + 0.55f);
				}
			}

			if (current->anim_speed > 0.f)
			{
				current->anim_speed /= new_velocity.length(true);

				new_velocity.x *= current->anim_speed;
				new_velocity.y *= current->anim_speed;
			}

			if (previous && current->time_delta > i::global_vars->interval_per_tick)
			{
				vector3d old_velocity = last->origin - previous->origin * (1.f / current->time_delta);

				if (!current->was_in_air && old_velocity.valid())
				{
					float current_direction = math::normalize(math::rad_to_deg(std::atan2(new_velocity.y, new_velocity.x)));
					float previous_direction = math::normalize(math::rad_to_deg(std::atan2(old_velocity.y, old_velocity.x)));

					float average_direction = current_direction - previous_direction;
					average_direction = math::deg_to_rad(math::normalize(current_direction + average_direction * 0.5f));

					float direction_cos = std::cos(average_direction);
					float dirrection_sin = std::sin(average_direction);

					float velocity_speed = new_velocity.length(true);

					new_velocity.x = direction_cos * velocity_speed;
					new_velocity.y = dirrection_sin * velocity_speed;
				}
			}

			if (!(current->player->flags() & fl_onground))
			{
				static auto sv_gravity = i::convar->find_convar("sv_gravity");
				float fixed_timing = std::clamp(current->time_delta, i::global_vars->interval_per_tick, 1.f);
				new_velocity.z -= sv_gravity->get_float() * fixed_timing * 0.5f;
			}
			else
				new_velocity.z = 0.f;
		}

		if (current->player->flags() & fl_onground && new_velocity.length(false) > 0.f && current->layers[original_side][6].weight <= 0.f)
			new_velocity.reset();

		current->velocity = new_velocity;

		// cycle goes to .5 only when player in air
		// and jump animation finished
		// after .5 landing animation will be played
		if (current->layers[original_side][4].cycle < 0.5f)
		{
			if (!(current->flags & fl_onground) || !(last->flags & fl_onground))
			{
				current->land_time = current->sim_time - (current->layers[original_side][4].cycle / current->layers[original_side][4].playback_rate);
				current->landing = current->land_time >= current->old_simtime;
			}
		}

		if (current->landing && !current->landed)
		{
			if (current->land_time >= current->old_simtime)
			{
				current->landed = true;
				current->on_ground = true;
			}
			else
				current->on_ground = last->flags & fl_onground;
		}
	}

	update_animations(0);
	*player->get_animation_state() = old_state;

	update_animations(-1);
	*player->get_animation_state() = old_state;

	update_animations(1);
	*player->get_animation_state() = old_state;

	update_animations(2);
	old_record->restore(player);
	delete old_record;

	//store_resolver_data(player, current);
}

c_lag_compensation* lag_compensation = new c_lag_compensation();