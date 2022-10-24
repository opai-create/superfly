#pragma once
#include <memory>

class c_animationlayer
{
public:
	bool client_blend;
	float blend_in;
	void* studio_hdr;
	int dispatch_sequence;
	int second_dispatch_sequence;
	uint32_t order;
	uint32_t sequence;
	float prev_cycle;
	float weight;
	float weight_delta_rate;
	float playback_rate;
	float cycle;
	void* owner;
	char  pad_0038[4];
};