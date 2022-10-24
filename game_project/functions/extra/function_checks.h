#pragma once

namespace misc_checks
{
	bool is_able_to_shoot(int shift_ticks = 0, bool revolver = false);
	bool is_firing(int shift_ticks = 0);
}