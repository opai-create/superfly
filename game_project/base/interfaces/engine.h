#pragma once
#include "../tools/math.h"
#include "../tools/memory.h"

#pragma region flow_type
#define flow_outgoing 0
#define flow_incoming 1
#define max_flows 2	 
#pragma endregion

class i_net_channel_info
{
public:
	enum
	{
		generic = 0,	// must be first and is default group
		localplayer,	// bytes for local player entity update
		otherplayers,	// bytes for other players update
		entities,		// all other entity bytes
		sounds,			// game sounds
		events,			// event messages
		tempents,		// temp entities
		usermessages,	// user messages
		entmessages,	// entity messages
		voice,			// voice data
		stringtable,	// a stringtable update
		move,			// client move cmds
		stringcmd,		// string command
		signon,			// various signondata
		total			// must be last and is not a real group
	};

	virtual const char* get_name(void) const = 0;	// get channel name
	virtual const char* get_address(void) const = 0; // get channel IP address as string
	virtual float get_time(void) const = 0; // current net time
	virtual float get_time_connected(void) const = 0; // get connection time in seconds
	virtual int get_buffer_size(void) const = 0; // netchannel packet history size
	virtual int get_data_rate(void) const = 0; // send data rate in byte/sec

	virtual bool is_loopback(void) const = 0; // true if loopback channel
	virtual bool is_timing_out(void) const = 0; // true if timing out
	virtual bool is_playback(void) const = 0; // true if demo playback

	virtual float get_latency(int flow) const = 0; // current latency (RTT), more accurate but jittering
	virtual float get_avg_latency(int flow) const = 0; // average packet latency in seconds
};

class i_netchannel : public i_net_channel_info
{

};

class c_netchan
{
public:
	char pad_0000[0x18];

	int out_sequence_nr;
	int in_sequence_nr;
	int out_sequence_nr_ack;

	int out_reliable_state;
	int in_reliable_state;

	int choked_packets;

	void send_datagram()
	{
		using fn = int(__thiscall*)(void*, void*);
		memory::vfunc<fn>(this, 46)(this, 0);
	}
};

class c_clientstate
{
public:
	// CBaseClientState
	char pad_0000[156];

	c_netchan* net_channel_ptr;

	char pad_00A0[104];

	int signon_state;

	char pad_010C[8];

	float next_cmd_time;

	int server_count;
	int current_sequence;

	char pad_0120[76];

	int server_tick;
	int client_tick;

	int delta_tick;

	char pad_0178[528];

	int max_clients;

	char pad_0390[0x498C];

	// CClientState
	float last_server_tick_time;

	bool in_simulation;

	char pad7[0x3];

	int old_tick_count;

	float tick_remainder;

	float frame_time;

	int last_outgoing_command;
	int choked_commands;

	int last_command_ack;
	int last_server_tick;
	int command_ack;

	char pad_4D40[80];

	vector3d viewangles;

	char pad_4D9C[648];
};

struct player_info_t
{
	int64_t __pad0;

	union
	{
		int64_t xuid;
		struct
		{
			int xuid_low;
			int xuid_high;
		};
	};

	char name[128];

	int userid;

	char guid[33];

	unsigned int friend_id;

	char friend_name[128];

	bool fake_player;

	bool is_hltv;

	unsigned int custom_files[4];

	unsigned char files_downloaded;
};

class c_engine
{
public:
	i_net_channel_info* get_net_channel_info()
	{
		using fn = i_net_channel_info * (__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 78)(this);
	}

	void* get_bsp_tree_query()
	{
		using fn = void* (__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 43)(this);
	}

	int get_local_player()
	{
		using fn = int(__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 12)(this);
	}

	int get_player_for_user_id(int user_id)
	{
		using fn = int(__thiscall*)(PVOID, int);
		return memory::vfunc<fn>(this, 9)(this, user_id);
	}

	void get_player_info(int index, player_info_t* info)
	{
		using fn = void(__thiscall*)(c_engine*, int, player_info_t*);
		return memory::vfunc<fn>(this, 8)(this, index, info);
	}

	void get_screen_size(int& width, int& height)
	{
		using fn = void(__thiscall*)(c_engine*, int&, int&);
		return memory::vfunc<fn>(this, 5)(this, width, height);
	}

	bool is_in_game()
	{
		using fn = bool(__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 26)(this);
	}

	bool is_connected()
	{
		using fn = bool(__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 27)(this);
	}

	bool is_playing_demo()
	{
		using fn = bool(__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 82)(this);
	}

	bool is_hltv()
	{
		using fn = bool(__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 93)(this);
	}

	void execute_cmd(const char* cmd)
	{
		using fn = void(__thiscall*)(c_engine*, const char*);
		return memory::vfunc<fn>(this, 108)(this, cmd);
	}

	void execute_cmd_unrestricted(const char* cmd, const char* flag = 0)
	{
		using fn = void(__thiscall*)(c_engine*, const char*, const char*);
		return memory::vfunc<fn>(this, 114)(this, cmd, flag);
	}

	void set_view_angles(vector3d& angles)
	{
		using fn = void(__thiscall*)(c_engine*, vector3d&);
		return memory::vfunc<fn>(this, 19)(this, angles);
	}

	void get_view_angles(vector3d& angles)
	{
		using fn = void(__thiscall*)(void*, vector3d&);
		return memory::vfunc<fn>(this, 18)(this, angles);
	}

	matrix3x4_t& world_to_screen_matrix()
	{
		using fn = matrix3x4_t & (__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 37)(this);
	}

	bool is_taking_screenshot()
	{
		using fn = bool(__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 92)(this);
	}

	const char* get_level_name()
	{
		using fn = const char* (__thiscall*)(c_engine*);
		return memory::vfunc<fn>(this, 53)(this);
	}
};