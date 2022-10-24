#pragma once

namespace movement
{
	extern float max_speed;

	void change_speed(float max_speed);
	void stop_move();
	void stop_on_walk(float max_speed);
	void run();
}