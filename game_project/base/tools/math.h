#pragma once
#include <cmath>
#include <corecrt_math.h>

class vector3d;

struct matrix3x4_t;

namespace math
{
	float rad_to_deg(float radian);
	float deg_to_rad(float degree);

	void vector_transform(vector3d in1, const matrix3x4_t& in2, vector3d& out);

	void vector_to_angles(vector3d forward, vector3d& angles);
	void vector_to_angles(vector3d& forward, vector3d& up, vector3d& angles);
	void angle_to_vectors(const vector3d& angles, vector3d& forward);
	void angle_to_vectors(const vector3d& angles, vector3d* forward, vector3d* right = nullptr, vector3d* up = nullptr);

	vector3d angle_from_vectors(vector3d a, vector3d b);

	float normalize(const float& ang);
	vector3d normalize(const vector3d& ang);

	float interpolate(const float& from, const float& to, const float& percent);
	vector3d interpolate(const vector3d& from, const vector3d& to, const float& percent);

	int time_to_ticks(float time);
	float ticks_to_time(int ticks);

	vector3d vector_rotate(const vector3d& in1, const vector3d& in2);
	void vector_i_transform(const vector3d& in1, const matrix3x4_t& in2, vector3d& out);
	void vector_i_rotate(const vector3d& in1, const matrix3x4_t& in2, vector3d& out);
	bool intersect_line_with_bb(vector3d& start, vector3d& end, vector3d& min, vector3d& max);

	float segment_to_segment(const vector3d& s1, const vector3d& s2, vector3d& k1, vector3d& k2);

	void contact_transforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);

	void random_seed(uint32_t seed);
	float random_float(float min, float max);
	int random_int(int min, int max);

	void vector_multiply(const vector3d& start, float scale, const vector3d& direction, vector3d& dest);
}

class quaternion
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;
};

class vector4d
{
public:
	float x = 0.f, y = 0.f, w = 0.f, h = 0.f;

	vector4d() {}
	vector4d(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) { }
};

class rect2d
{
public:
	float w = 0.f, h = 0.f;

	rect2d() {}
	rect2d(float w, float h) : w(w), h(h) { }

	inline void reset()
	{
		w = 0.f;
		h = 0.f;
	}

	inline bool invalid()
	{
		return w != 0.f && h != 0.f;
	}
};

class vector2d
{
public:
	float x = 0.f, y = 0.f;

	vector2d() {}
	vector2d(float x, float y) : x(x), y(y) { }

	bool operator==(const vector2d& v)
	{
		return this->x == v.x
			&& this->y == v.y;
	}

	vector2d operator+(const vector2d& v)
	{
		return vector2d(this->x + v.x, this->y + v.y);
	}

	vector2d operator-(const vector2d& v)
	{
		return vector2d(this->x - v.x, this->y - v.y);
	}

	vector2d operator/(const vector2d& v)
	{
		return vector2d(this->x / v.x, this->y / v.y);
	}

	vector2d operator*(const vector2d& v)
	{
		return vector2d(this->x * v.x, this->y * v.y);
	}

	vector2d operator+=(const vector2d& v)
	{
		this->x += v.x;
		this->y += v.y;
		return *this;
	}

	vector2d operator-=(const vector2d& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}

	vector2d operator/=(const vector2d& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		return *this;
	}

	vector2d operator*=(const vector2d& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		return *this;
	}

	inline float scalar_product(const vector2d& v)
	{
		return this->x * v.x + this->y * v.y;
	}

	inline float distance_to_vector(const vector2d& p)
	{
		return std::sqrt(
			std::pow(p.x - this->x, 2) +
			std::pow(p.y - this->y, 2)
		);
	}

	inline float length()
	{
		return std::sqrt(
			std::pow(this->x, 2) +
			std::pow(this->y, 2)
		);
	}
};

class vector3d
{
public:
	float x = 0.f, y = 0.f, z = 0.f;

	vector3d() {}
	vector3d(float x, float y, float z) : x(x), y(y), z(z) {}

	bool operator==(const vector3d& v)
	{
		return this->x == v.x
			&& this->y == v.y
			&& this->z == v.z;
	}

	bool operator!=(const vector3d& v)
	{
		return this->x != v.x
			&& this->y != v.y
			&& this->z != v.z;
	}

	vector3d operator+(const vector3d& v) const
	{
		return vector3d(this->x + v.x, this->y + v.y, this->z + v.z);
	}

	vector3d operator-() const
	{
		return vector3d(-this->x, -this->y, -this->z);
	}

	vector3d operator-(const vector3d& v) const
	{
		return vector3d(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	vector3d operator/(const vector3d& v) const
	{
		return vector3d(this->x / v.x, this->y / v.y, this->z / v.z);
	}

	vector3d operator*(const vector3d& v) const
	{
		return vector3d(this->x * v.x, this->y * v.y, this->z * v.z);
	}

	vector3d operator*(const float& v) const
	{
		return vector3d(this->x * v, this->y * v, this->z * v);
	}

	vector3d operator+=(const vector3d& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}

	vector3d operator-=(const vector3d& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}

	vector3d operator/=(const vector3d& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		return *this;
	}

	vector3d operator*=(const vector3d& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		return *this;
	}

	vector3d operator*=(const float& fl)
	{
		this->x *= fl;
		this->y *= fl;
		this->z *= fl;
		return *this;
	}

	vector3d operator/(const float& fl)
	{
		return vector3d(this->x / fl, this->y / fl, this->z / fl);
	}

	vector3d operator/=(const float& fl)
	{
		this->x *= fl;
		this->y *= fl;
		this->z *= fl;
		return *this;
	}

	inline void reset()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	inline bool valid()
	{
		return *this != vector3d(0.f, 0.f, 0.f);
	}

	void init(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline float& operator[](int i) 
	{
		return ((float*)this)[i];
	}

	inline float dot(float* v)
	{
		return this->x * v[0] + this->y * v[1] + this->z * v[2];
	}

	inline float dot(const vector3d& v)
	{
		return this->x * v.x + this->y * v.y + this->z * v.z;
	}

	inline float length(bool ignore_z)
	{
		if (ignore_z)
			return std::sqrt(this->x * this->x + this->y * this->y);

		return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	inline float distance_to_vector(const vector3d& p)
	{
		return std::sqrt(
			std::pow(p.x - this->x, 2) +
			std::pow(p.y - this->y, 2) +
			std::pow(p.z - this->z, 2));
	}

	inline float length_sqr()
	{
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}

	inline float normalized_float()
	{
		float len = length(false);
		(*this) /= (length(false) + std::numeric_limits<float>::epsilon());
		return len;
	}

	inline vector3d normalized() const
	{
		auto vec = *this;
		vec.normalized_float();
		return vec;
	}
};

class vertex
{
public:
	vector2d position;
	vector2d tex_position;

	vertex() { }
	vertex(const vector2d& pos, const vector2d& coord = vector2d(0, 0))
		: position(pos), tex_position(coord)
	{ }
};

struct matrix3x4_t
{
	matrix3x4_t() = default;

	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		mat[0][0] = m00;	mat[0][1] = m01; mat[0][2] = m02; mat[0][3] = m03;
		mat[1][0] = m10;	mat[1][1] = m11; mat[1][2] = m12; mat[1][3] = m13;
		mat[2][0] = m20;	mat[2][1] = m21; mat[2][2] = m22; mat[2][3] = m23;
	}

	inline void set_origin(const vector3d& p)
	{
		mat[0][3] = p.x;
		mat[1][3] = p.y;
		mat[2][3] = p.z;
	}

	inline vector3d get_origin()
	{
		vector3d vecRet(mat[0][3], mat[1][3], mat[2][3]);
		return vecRet;
	}

	void quaternion_matrix(const quaternion& q)
	{
		mat[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
		mat[1][0] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
		mat[2][0] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

		mat[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
		mat[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
		mat[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

		mat[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
		mat[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
		mat[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

		mat[0][3] = 0.0f;
		mat[1][3] = 0.0f;
		mat[2][3] = 0.0f;
	}

	void quaternion_matrix(const quaternion& q, const vector3d& pos)
	{
		quaternion_matrix(q);

		mat[0][3] = pos.x;
		mat[1][3] = pos.y;
		mat[2][3] = pos.z;
	}

	matrix3x4_t contact_transforms(matrix3x4_t in) const
	{
		matrix3x4_t out = {};

		out.mat[0][0] = mat[0][0] * in.mat[0][0] + mat[0][1] * in.mat[1][0] + mat[0][2] * in.mat[2][0];
		out.mat[0][1] = mat[0][0] * in.mat[0][1] + mat[0][1] * in.mat[1][1] + mat[0][2] * in.mat[2][1];
		out.mat[0][2] = mat[0][0] * in.mat[0][2] + mat[0][1] * in.mat[1][2] + mat[0][2] * in.mat[2][2];
		out.mat[0][3] = mat[0][0] * in.mat[0][3] + mat[0][1] * in.mat[1][3] + mat[0][2] * in.mat[2][3] + mat[0][3];
		out.mat[1][0] = mat[1][0] * in.mat[0][0] + mat[1][1] * in.mat[1][0] + mat[1][2] * in.mat[2][0];
		out.mat[1][1] = mat[1][0] * in.mat[0][1] + mat[1][1] * in.mat[1][1] + mat[1][2] * in.mat[2][1];
		out.mat[1][2] = mat[1][0] * in.mat[0][2] + mat[1][1] * in.mat[1][2] + mat[1][2] * in.mat[2][2];
		out.mat[1][3] = mat[1][0] * in.mat[0][3] + mat[1][1] * in.mat[1][3] + mat[1][2] * in.mat[2][3] + mat[1][3];
		out.mat[2][0] = mat[2][0] * in.mat[0][0] + mat[2][1] * in.mat[1][0] + mat[2][2] * in.mat[2][0];
		out.mat[2][1] = mat[2][0] * in.mat[0][1] + mat[2][1] * in.mat[1][1] + mat[2][2] * in.mat[2][1];
		out.mat[2][2] = mat[2][0] * in.mat[0][2] + mat[2][1] * in.mat[1][2] + mat[2][2] * in.mat[2][2];
		out.mat[2][3] = mat[2][0] * in.mat[0][3] + mat[2][1] * in.mat[1][3] + mat[2][2] * in.mat[2][3] + mat[2][3];

		return out;
	}

	void angle_matrix(const vector3d& angles)
	{
		float sin_x = std::sin(math::deg_to_rad(angles.x));
		float cos_x = std::cos(math::deg_to_rad(angles.x));

		float sin_y = std::sin(math::deg_to_rad(angles.y));
		float cos_y = std::cos(math::deg_to_rad(angles.y));

		float sin_z = std::sin(math::deg_to_rad(angles.z));
		float cos_z = std::cos(math::deg_to_rad(angles.z));

		mat[0][0] = cos_x * cos_y;
		mat[1][0] = cos_x * sin_y;
		mat[2][0] = -sin_x;

		float crcy = cos_z * cos_y;
		float crsy = cos_z * sin_y;
		float srcy = sin_z * cos_y;
		float srsy = sin_z * sin_y;

		mat[0][1] = sin_x * srcy - crsy;
		mat[1][1] = sin_x * srsy + crcy;
		mat[2][1] = sin_z * cos_x;

		mat[0][2] = (sin_x * crcy + srsy);
		mat[1][2] = (sin_x * crsy - srcy);
		mat[2][2] = cos_z * cos_x;

		mat[0][3] = 0.f;
		mat[1][3] = 0.f;
		mat[2][3] = 0.f;
	}

	void angle_matrix(const vector3d& angles, const vector3d& position)
	{
		this->angle_matrix(angles);
		this->set_origin(position);
	}

	float mat[3][4] = {};
};

class __declspec(align(16)) vector_aligned : public vector3d
{
public:
	vector_aligned() { }

	vector_aligned(float x, float y, float z)
	{
		init(x, y, z);
	}

	explicit vector_aligned(const vector3d& v)
	{
		init(v.x, v.y, v.z);
	}

	vector_aligned& operator=(const vector3d& v)
	{
		init(v.x, v.y, v.z);
		return *this;
	}

	vector_aligned& operator=(const vector_aligned& v)
	{
		init(v.x, v.y, v.z);
		return *this;
	}

	float w = 0.f;
};