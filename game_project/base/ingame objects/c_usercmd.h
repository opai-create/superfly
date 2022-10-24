#pragma once
#include "../tools/math.h"
#include "../other/checksum_crc.h"

#pragma region cmd_buttons
#define in_attack (1 << 0)
#define in_jump (1 << 1)
#define in_duck (1 << 2)
#define in_forward (1 << 3)
#define in_back (1 << 4)
#define in_use (1 << 5)
#define in_cancel (1 << 6)
#define in_left (1 << 7)
#define in_right (1 << 8)
#define in_moveleft (1 << 9)
#define in_moveright (1 << 10)
#define in_attack2 (1 << 11)
#define in_run (1 << 12)
#define in_reload (1 << 13)
#define in_alt1 (1 << 14)
#define in_alt2 (1 << 15)
#define in_score (1 << 16)
#define in_speed (1 << 17)
#define in_walk (1 << 18)
#define in_zoom (1 << 19)
#define in_weapon1 (1 << 20)
#define in_weapon2 (1 << 21)
#define in_bullrush (1 << 22)
#define in_grenade1 (1 << 23)
#define in_grenade2 (1 << 24)
#define in_lookspin (1 << 25)
#pragma endreigon

class c_usercmd
{
public:
	c_usercmd()
	{
		std::memset(this, 0, sizeof(*this));
	};
	virtual ~c_usercmd() {};

	crc32_t get_check_sum(void) const
	{
		crc32_t crc;
		crc32_Init(&crc);

		crc32_process_buffer(&crc, &command_number, sizeof(command_number));
		crc32_process_buffer(&crc, &tickcount, sizeof(tickcount));
		crc32_process_buffer(&crc, &viewangles, sizeof(viewangles));
		crc32_process_buffer(&crc, &aimdirection, sizeof(aimdirection));
		crc32_process_buffer(&crc, &forwardmove, sizeof(forwardmove));
		crc32_process_buffer(&crc, &sidemove, sizeof(sidemove));
		crc32_process_buffer(&crc, &upmove, sizeof(upmove));
		crc32_process_buffer(&crc, &buttons, sizeof(buttons));
		crc32_process_buffer(&crc, &impulse, sizeof(impulse));
		crc32_process_buffer(&crc, &weaponselect, sizeof(weaponselect));
		crc32_process_buffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		crc32_process_buffer(&crc, &random_seed, sizeof(random_seed));
		crc32_process_buffer(&crc, &mousedx, sizeof(mousedx));
		crc32_process_buffer(&crc, &mousedy, sizeof(mousedy));

		crc32_final(&crc);

		return crc;
	}

	int command_number;
	int tickcount;
	vector3d viewangles;
	vector3d aimdirection;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	char impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool predicted;
	char pad_0x4C[0x18];
};

class c_cmd_ctx
{
public:
	bool			needsprocessing;
	c_usercmd		cmd;
	int				command_number;
};