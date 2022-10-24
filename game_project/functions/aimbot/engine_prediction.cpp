#include "engine_prediction.h"

#include <thread>

// fix issues with predicted netvars (like aimpunch, etc)
namespace net_compression
{
    std::array<stored_data_t, 150> data = {};

    void reset()
    {
        data.fill(stored_data_t());
    }

    void store()
    {
        if (!g::local() || !g::local()->is_alive())
        {
            reset();
            return;
        }

        int tick_base = g::local()->tick_base();

        stored_data_t& d = data[tick_base % 150];
        d.tick_base = tick_base;
        d.punch = g::local()->aim_punch_angle();
        d.punch_vel = g::local()->aim_punch_angle_vel();
        d.view_offset = g::local()->view_offset();
        d.vel_modifier = g::local()->velocity_modifier();
    }

    void set(int stage)
    {
        if (stage != frame_net_update_end)
            return;

        if (!g::local() || !g::local()->is_alive())
        {
            reset();
            return;
        }

        int tick_base = g::local()->tick_base();

        stored_data_t& d = data[tick_base % 150];
        if (d.tick_base != g::local()->tick_base())
            return;

        auto punch_delta = g::local()->aim_punch_angle() - d.punch;
        auto punch_vel_delta = g::local()->aim_punch_angle_vel() - d.punch_vel;
        auto view_delta = g::local()->view_offset() - d.view_offset;

        if (std::abs(punch_delta.x) < 0.03125f &&
            std::abs(punch_delta.y) < 0.03125f &&
            std::abs(punch_delta.z) < 0.03125f)
            g::local()->aim_punch_angle() = d.punch;

        if (std::abs(punch_vel_delta.x) < 0.03125f &&
            std::abs(punch_vel_delta.y) < 0.03125f &&
            std::abs(punch_vel_delta.z) < 0.03125f)
            g::local()->aim_punch_angle_vel() = d.punch_vel;

        if (std::abs(view_delta.x) < 0.03125f &&
            std::abs(view_delta.y) < 0.03125f &&
            std::abs(view_delta.z) < 0.03125f)
            g::local()->view_offset() = d.view_offset;

        if (std::abs(g::local()->velocity_modifier() - d.vel_modifier) <= 0.00625f)
            g::local()->velocity_modifier() = d.vel_modifier;
    }
}

namespace engine_prediction
{
    float server_time = 0.f;
    float frame_time = 0.f;

    uint32_t* pred_seed = nullptr;
    c_csplayer** pred_player = nullptr;

	void start()
	{
        if (!pred_seed)
            pred_seed = *patterns[pattern_t::prediction_random_seed].as<uint32_t**>();

        if (!pred_player)
            pred_player = *patterns[pattern_t::prediction_player].as<c_csplayer***>();

        *pred_seed = game_fn::md5_pseudo_random(g::cmd->command_number) & 0x7FFFFFFF;
        *pred_player = g::local();

        server_time = i::global_vars->cur_time;
        frame_time = i::global_vars->frame_time;

        i::global_vars->cur_time = math::ticks_to_time(g::tick_base);
        i::global_vars->frame_time = i::global_vars->interval_per_tick;

        i::game_movement->start_track_prediction_errors(g::local());
        i::move_helper->set_host(g::local());

        c_movedata data;
        memset(&data, 0, sizeof(data));

        i::prediction->setup_move(g::local(), g::cmd, i::move_helper, &data);

        i::game_movement->process_movement(g::local(), &data);
        i::prediction->finish_move(g::local(), g::cmd, &data);
	}

	void end()
	{
        i::game_movement->finish_track_prediction_errors(g::local());
        i::move_helper->set_host(nullptr);

        *pred_seed = -1;
        *pred_player = nullptr;

        i::global_vars->cur_time = server_time;
        i::global_vars->frame_time = frame_time;
	}
}