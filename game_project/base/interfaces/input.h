#pragma once

struct verified_cmd_t
{
	c_usercmd cmd;
	crc32_t	crc;
};

class c_input
{
public:
	char pad0[0xC];
	bool trackir_available;
	bool mouse_initialized;
	bool mouse_active;
	char pad1[0x9A];
	bool camera_in_third_person;
	char pad2[0x2];
	vector3d camera_offset;
	char pad3[0x38];
	c_usercmd* commands;
	verified_cmd_t* verified_commands;

	c_usercmd* get_user_cmd(int sequence_number)
	{
		return &commands[sequence_number % 150];
	}

	verified_cmd_t* get_verified_user_cmd(int sequence_number)
	{
		return &verified_commands[sequence_number % 150];
	}
};