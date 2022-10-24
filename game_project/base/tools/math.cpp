#include "math.h"

#include "../sdk.h"
#include "../global_variables.h"

#include "../other/game_functions.h"

namespace math
{
	float rad_to_deg(float radian)
	{
		return (radian * 180.f) / static_cast<float>(M_PI);
	}

	float deg_to_rad(float degree)
	{
		return (degree * static_cast<float>(M_PI)) / 180.f;
	}

	void vector_transform(vector3d in1, const matrix3x4_t& in2, vector3d& out)
	{
		out = vector3d(
			in1.dot(vector3d(in2.mat[0][0], in2.mat[0][1], in2.mat[0][2])) + in2.mat[0][3],
			in1.dot(vector3d(in2.mat[1][0], in2.mat[1][1], in2.mat[1][2])) + in2.mat[1][3],
			in1.dot(vector3d(in2.mat[2][0], in2.mat[2][1], in2.mat[2][2])) + in2.mat[2][3]);
	}

	void vector_to_angles(vector3d forward, vector3d& angles)
	{
		float tmp, yaw, pitch;

		if (forward.y == 0.f && forward.x == 0.f)
		{
			yaw = 0.f;
			if (forward.z > 0.f)
				pitch = 270.f;
			else
				pitch = 90.f;
		}
		else
		{
			yaw = math::rad_to_deg(std::atan2(forward.y, forward.x));
			if (yaw < 0.f)
				yaw += 360.f;

			tmp = forward.length(true);
			pitch = math::rad_to_deg(std::atan2(-forward.z, tmp));
			if (pitch < 0.f)
				pitch += 360.f;
		}

		angles.x = pitch;
		angles.y = yaw;
		angles.x = 0;
	}

	vector3d cross_product(const vector3d& a, const vector3d& b)
	{
		return vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	void vector_to_angles(vector3d& forward, vector3d& up, vector3d& angles)
	{
		vector3d left = cross_product(up, forward);

		float forward_l = forward.length(true);

		if (forward_l > 0.001f)
		{
			angles.x = std::atan2(-forward.z, forward_l) * 180.f / M_PI;
			angles.y = std::atan2(forward.y, forward.x) * 180.f / M_PI;

			float up_z = (left.y * forward.x) - (left.x * forward.y);
			angles.z = std::atan2(left.z, up_z) * 180 / M_PI;
		}
		else
		{
			angles.x = std::atan2(-forward.z, forward_l) * 180.f / M_PI;
			angles.y = std::atan2(-left.x, left.y) * 180.f / M_PI;
			angles.z = 0.f;
		}
	}

	void angle_to_vectors(const vector3d& angles, vector3d& forward)
	{
		float sin_y = std::sin(deg_to_rad(angles.y));
		float cos_y = std::cos(deg_to_rad(angles.y));

		float sin_x = std::sin(deg_to_rad(angles.x));
		float cos_x = std::cos(deg_to_rad(angles.x));

		forward.x = cos_x * cos_y;
		forward.y = cos_x * sin_y;
		forward.z = -sin_x;
	}

	void angle_to_vectors(const vector3d& angles, vector3d* forward, vector3d* right, vector3d* up)
	{
		float cp = std::cos(deg_to_rad(angles.x)), sp = std::sin(deg_to_rad(angles.x));
		float cy = std::cos(deg_to_rad(angles.y)), sy = std::sin(deg_to_rad(angles.y));
		float cr = std::cos(deg_to_rad(angles.z)), sr = std::sin(deg_to_rad(angles.z));

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
			right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
			right->z = -1.f * sr * cp;
		}

		if (up)
		{
			up->x = cr * sp * cy + -sr * -sy;
			up->y = cr * sp * sy + -sr * cy;
			up->z = cr * cp;
		}
	}

	vector3d angle_from_vectors(vector3d a, vector3d b)
	{
		vector3d angles{};

		vector3d delta = a - b;
		float hyp = delta.length(true);

		// 57.295f - pi in degrees
		angles.y = std::atan(delta.y / delta.x) * 57.295f;
		angles.x = std::atan(-delta.z / hyp) * -57.295f;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	float normalize(const float& ang)
	{
		return std::remainderf(ang, 360.f);
	}

	vector3d normalize(const vector3d& ang)
	{
		return vector3d(std::remainderf(ang.x, 178.f),
			std::remainderf(ang.y, 360.f),
			std::remainderf(ang.z, 100.f));
	}

	float interpolate(const float& from, const float& to, const float& percent)
	{
		return to * percent + from * (1.f - percent);
	}

	vector3d interpolate(const vector3d& from, const vector3d& to, const float& percent)
	{
		return to * percent + from * (1.f - percent);
	}

	int time_to_ticks(float time)
	{
		return 0.5f + time / i::global_vars->interval_per_tick;
	}

	float ticks_to_time(int ticks)
	{
		return ticks * i::global_vars->interval_per_tick;
	}

	vector3d vector_rotate(vector3d in1, matrix3x4_t in2)
	{
		return vector3d(
			in1.dot(vector3d(in2.mat[0][0], in2.mat[0][1], in2.mat[0][2])),
			in1.dot(vector3d(in2.mat[1][0], in2.mat[1][1], in2.mat[1][2])),
			in1.dot(vector3d(in2.mat[2][0], in2.mat[2][1], in2.mat[2][2])));
	}

	vector3d vector_rotate(const vector3d& in1, const vector3d& in2)
	{
		matrix3x4_t matrix = {  };
		matrix.angle_matrix(in1);
		return vector_rotate(in1, matrix);
	}

	void vector_i_transform(const vector3d& in1, const matrix3x4_t& in2, vector3d& out)
	{
		out.x = (in1.x - in2.mat[0][3]) * in2.mat[0][0] + (in1.y - in2.mat[1][3]) * in2.mat[1][0] + (in1.z - in2.mat[2][3]) * in2.mat[2][0];
		out.y = (in1.x - in2.mat[0][3]) * in2.mat[0][1] + (in1.y - in2.mat[1][3]) * in2.mat[1][1] + (in1.z - in2.mat[2][3]) * in2.mat[2][1];
		out.z = (in1.x - in2.mat[0][3]) * in2.mat[0][2] + (in1.y - in2.mat[1][3]) * in2.mat[1][2] + (in1.z - in2.mat[2][3]) * in2.mat[2][2];
	}

	void vector_i_rotate(const vector3d& in1, const matrix3x4_t& in2, vector3d& out)
	{
		out.x = in1.x * in2.mat[0][0] + in1.y * in2.mat[1][0] + in1.z * in2.mat[2][0];
		out.y = in1.x * in2.mat[0][1] + in1.y * in2.mat[1][1] + in1.z * in2.mat[2][1];
		out.z = in1.x * in2.mat[0][2] + in1.y * in2.mat[1][2] + in1.z * in2.mat[2][2];
	}

	bool intersect_line_with_bb(vector3d& start, vector3d& end, vector3d& min, vector3d& max) 
	{
		float d1, d2, f;
		auto start_solid = true;
		auto t1 = -1.0f, t2 = 1.0f;

		const float s[3] = { start.x, start.y, start.z };
		const float e[3] = { end.x, end.y, end.z };
		const float mi[3] = { min.x, min.y, min.z };
		const float ma[3] = { max.x, max.y, max.z };

		for (auto i = 0; i < 6; i++) {
			if (i >= 3) {
				const auto j = i - 3;

				d1 = s[j] - ma[j];
				d2 = d1 + e[j];
			}
			else {
				d1 = -s[i] + mi[i];
				d2 = d1 - e[i];
			}

			if (d1 > 0.0f && d2 > 0.0f)
				return false;

			if (d1 <= 0.0f && d2 <= 0.0f)
				continue;

			if (d1 > 0)
				start_solid = false;

			if (d1 > d2) {
				f = d1;
				if (f < 0.0f)
					f = 0.0f;

				f /= d1 - d2;
				if (f > t1)
					t1 = f;
			}
			else {
				f = d1 / (d1 - d2);
				if (f < t2)
					t2 = f;
			}
		}

		return start_solid || (t1 < t2&& t1 >= 0.0f);
	}

	float segment_to_segment(const vector3d& s1, const vector3d& s2, vector3d& k1, vector3d& k2)
	{
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.dot(u);
		const auto b = u.dot(v);
		const auto c = v.dot(v);
		const auto d = u.dot(w);
		const auto e = v.dot(w);
		const auto D = a * c - b * b;
		float sn, sd = D;
		float tn, td = D;

		if (D < epsilon) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if (sn < 0.0) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if (sn > sd) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if (tn < 0.0) {
			tn = 0.0;

			if (-d < 0.0)
				sn = 0.0;
			else if (-d > a)
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if (tn > td) {
			tn = td;

			if (-d + b < 0.0)
				sn = 0;
			else if (-d + b > a)
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = std::abs(sn) < epsilon ? 0.0 : sn / sd;
		const float tc = std::abs(tn) < epsilon ? 0.0 : tn / td;
		
		auto dp = w + u * sc - v * tc;
		return std::sqrt(dp.dot(dp));
	}

	void contact_transforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out)
	{
		if (&in1 == &out) 
		{
			matrix3x4_t in1b = in1;
			contact_transforms(in1b, in2, out);
			return;
		}

		if (&in2 == &out) 
		{
			matrix3x4_t in2b = in2;
			contact_transforms(in1, in2b, out);
			return;
		}

		out.mat[0][0] = in1.mat[0][0] * in2.mat[0][0] + in1.mat[0][1] * in2.mat[1][0] + in1.mat[0][2] * in2.mat[2][0];
		out.mat[0][1] = in1.mat[0][0] * in2.mat[0][1] + in1.mat[0][1] * in2.mat[1][1] + in1.mat[0][2] * in2.mat[2][1];
		out.mat[0][2] = in1.mat[0][0] * in2.mat[0][2] + in1.mat[0][1] * in2.mat[1][2] + in1.mat[0][2] * in2.mat[2][2];
		out.mat[0][3] = in1.mat[0][0] * in2.mat[0][3] + in1.mat[0][1] * in2.mat[1][3] + in1.mat[0][2] * in2.mat[2][3] + in1.mat[0][3];

		out.mat[1][0] = in1.mat[1][0] * in2.mat[0][0] + in1.mat[1][1] * in2.mat[1][0] + in1.mat[1][2] * in2.mat[2][0];
		out.mat[1][1] = in1.mat[1][0] * in2.mat[0][1] + in1.mat[1][1] * in2.mat[1][1] + in1.mat[1][2] * in2.mat[2][1];
		out.mat[1][2] = in1.mat[1][0] * in2.mat[0][2] + in1.mat[1][1] * in2.mat[1][2] + in1.mat[1][2] * in2.mat[2][2];
		out.mat[1][3] = in1.mat[1][0] * in2.mat[0][3] + in1.mat[1][1] * in2.mat[1][3] + in1.mat[1][2] * in2.mat[2][3] + in1.mat[1][3];

		out.mat[2][0] = in1.mat[2][0] * in2.mat[0][0] + in1.mat[2][1] * in2.mat[1][0] + in1.mat[2][2] * in2.mat[2][0];
		out.mat[2][1] = in1.mat[2][0] * in2.mat[0][1] + in1.mat[2][1] * in2.mat[1][1] + in1.mat[2][2] * in2.mat[2][1];
		out.mat[2][2] = in1.mat[2][0] * in2.mat[0][2] + in1.mat[2][1] * in2.mat[1][2] + in1.mat[2][2] * in2.mat[2][2];
		out.mat[2][3] = in1.mat[2][0] * in2.mat[0][3] + in1.mat[2][1] * in2.mat[1][3] + in1.mat[2][2] * in2.mat[2][3] + in1.mat[2][3];
	}

	typedef void(*random_seed_fn)(UINT);
	random_seed_fn o_random_seed = 0;

	void random_seed(uint32_t seed) 
	{
		if (!o_random_seed)
			o_random_seed = (random_seed_fn)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed");
		o_random_seed(seed);
	}

	typedef float(*random_float_fn)(float, float);
	random_float_fn o_random_float;

	float random_float(float min, float max)
	{
		if (!o_random_float)
			o_random_float = (random_float_fn)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat");

		return o_random_float(min, max);
	}

	typedef int(*random_int_fn)(int, int);
	random_int_fn o_random_int;

	int random_int(int min, int max)
	{
		if (!o_random_int)
			o_random_int = (random_int_fn)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomInt");

		return o_random_int(min, max);
	}

	void vector_multiply(const vector3d& start, float scale, const vector3d& direction, vector3d& dest)
	{
		dest.x = start.x + direction.x * scale;
		dest.y = start.y + direction.y * scale;
		dest.z = start.z + direction.z * scale;
	}
}