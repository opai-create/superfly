#include "movement.h"

#include "../config_vars.h"

#include "../../base/sdk.h"
#include "../../base/global_variables.h"

#include "../../base/tools/math.h"

#include "../../base/ingame objects/c_usercmd.h"
#include "../../base/ingame objects/c_animstate.h"
#include "../../base/ingame objects/c_csplayer.h"
#include "../../base/ingame objects/c_basecombatweapon.h"

namespace movement
{
    float max_speed = 0.f;

    vector3d start_position = {};
    bool should_move = false;

    void auto_jump()
    {
        if (!g_cfg.misc.auto_jump)
            return;

        if (g::local()->flags() & fl_onground)
            return;

        g::cmd->buttons &= ~in_jump;
    }

    float get_move_rotation(unsigned int buttons)
    {
        float yaw_add = 0.f;
        bool back = buttons & in_back;
        bool forward = buttons & in_forward;
        bool left = buttons & in_moveright;
        bool right = buttons & in_moveleft;

        if (back)
        {
            yaw_add = -180.f;

            if (right)
                yaw_add -= 45.f;
            else if (left)
                yaw_add += 45.f;
        }
        else if (right)
        {
            yaw_add = 90.f;

            if (back)
                yaw_add += 45.f;
            else if (forward)
                yaw_add -= 45.f;
        }
        else if (left)
        {
            yaw_add = -90.f;

            if (back)
                yaw_add -= 45.f;
            else if (forward)
                yaw_add += 45.f;
        }
        else
            yaw_add = 0.f;

        return yaw_add;
    }

    void auto_strafe()
    {
        if (!g_cfg.misc.auto_strafe)
            return;

        if (g::cmd->buttons & in_speed)
            return;

        if (g::local()->flags() & fl_onground)
            return;

        vector3d velocity = g::local()->velocity();
        float velocity_angle = math::rad_to_deg(std::atan2(velocity.y, velocity.x));

        g::base_angle.y += get_move_rotation(g::cmd->buttons);

        g::cmd->forwardmove = g::cmd->sidemove = 0.f;

        float delta = math::normalize(g::base_angle.y - velocity_angle);
        g::cmd->sidemove = delta > 0.f ? -450.f : 450.f;
        g::base_angle.y -= delta;
    }

    void auto_peek()
    {
        vector3d origin = g::local()->origin();

        auto should_peek = [&]()
        {
            if (!(g::local()->flags() & fl_onground))
                return false;

            if (!g_cfg.binds[ap_b].toggled)
                return false;

            if (!start_position.valid())
                start_position = origin;

            if (g::cmd->buttons & in_attack)
                should_move = true;

            vector3d origin_delta = start_position - origin;
            float distance = start_position.distance_to_vector(origin);

            if (should_move)
            {
                if (distance <= 5.f)
                    return false;

                vector3d return_position = math::angle_from_vectors(origin, start_position);
                g::base_angle.y = return_position.y;

                g::cmd->forwardmove = 450.f;
                g::cmd->sidemove = 0.f;
            }

            return true;
        };

        if (!should_peek())
        {
            start_position.reset();
            should_move = false;
        }
    }

    void stop_move()
    {
        vector3d velocity = g::local()->velocity();
        float speed = velocity.length(true);

        static auto sv_accelerate = i::convar->find_convar("sv_accelerate");
        static auto sv_maxspeed = i::convar->find_convar("sv_maxspeed");

        float accelerate = sv_accelerate->get_float();
        float max_speed = sv_maxspeed->get_float();
        float surface_friction = g::local()->surface_friction();
        float max_accelspeed = accelerate * max_speed * surface_friction * i::global_vars->interval_per_tick;

        auto get_maximum_accelerate_speed = [&]
        {
            if (speed - max_accelspeed <= -1.f)
            {
                float speed_ratio = speed / (accelerate * i::global_vars->interval_per_tick);
                return max_accelspeed / speed_ratio;
            }

            return max_accelspeed;
        };

        vector3d velocity_angle = {};
        math::vector_to_angles(-velocity, velocity_angle);
        velocity_angle.y = g::cmd->viewangles.y - velocity_angle.y;

        vector3d forward = {};
        math::angle_to_vectors(velocity_angle, forward);

        float total_speed = get_maximum_accelerate_speed();
        g::cmd->forwardmove = forward.x * total_speed;
        g::cmd->sidemove = forward.y * total_speed;
    }

    void stop_on_walk(float max_speed)
    {
        float speed = g::local()->velocity().length(true);
        if (speed > max_speed)
            stop_move();
        else
            change_speed(max_speed);
    }

    // https://www.unknowncheats.me/forum/counterstrike-global-offensive/258323-autostop.html
    void fast_stop()
    {
        if (!g_cfg.misc.fast_stop)
            return;

        if (should_move)
            return;

        if (!(g::local()->flags() & fl_onground))
            return;

        vector3d velocity = g::local()->velocity();
        float speed = velocity.length(true);

        if (speed < 1.5f)
            return;

        bool pressing_move_keys = g::cmd->buttons & in_moveleft
            || g::cmd->buttons & in_moveright
            || g::cmd->buttons & in_back
            || g::cmd->buttons & in_forward;

        if (pressing_move_keys)
            return;

        stop_move();
    }

    void change_speed(float max_speed)
    {
        float sidemove = g::cmd->sidemove;
        float forwardmove = g::cmd->forwardmove;

        float move_speed = std::sqrt(std::pow(sidemove, 2) + std::pow(forwardmove, 2));
        if (move_speed > max_speed)
        {
            bool invalid_speed = max_speed + 1.f < g::local()->velocity().length(true);

            g::cmd->sidemove = invalid_speed ? 0.f : (sidemove / move_speed) * max_speed;
            g::cmd->forwardmove = invalid_speed ? 0.f : (forwardmove / move_speed) * max_speed;
        }
    }

    void slow_walk()
    {
        if (!g_cfg.binds[sw_b].toggled)
            return;

        if (!(g::local()->flags() & fl_onground))
            return;

        change_speed(max_speed / 3.f);
    }

    void run()
    {
        if (!g::in_game)
            return;

        if (!g::local()->is_alive())
            return;

        if (g::local()->move_type() == movetype_ladder
            || g::local()->move_type() == movetype_noclip)
            return;

        c_basecombatweapon* weapon = g::weapon();
        if (!weapon)
            return;

        weapon_info_t* info = g::weapon()->get_weapon_info();
        if (!info)
            return;

        max_speed = g::local()->is_scoped() ? info->max_speed_alt : info->max_speed;

        auto_jump();
        auto_strafe();
        auto_peek();
        slow_walk();
        fast_stop();
    }
}