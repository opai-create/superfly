#pragma once
#include "../tools/math.h"

#include <algorithm>

#undef max
#undef min

class color
{
public:

	color()
	{
		for (int i = 0; i < 4; i++)
			_color[i] = 255;
	}

	color(int color32)
	{
		*((int*)this) = color32;
	}

	color(int _r, int _g, int _b)
	{
		set(_r, _g, _b, 255);
	}

	color(int _r, int _g, int _b, int _a)
	{
		set(_r, _g, _b, _a);
	}

	void set(int _r, int _g, int _b, int _a = 255)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void get(int& _r, int& _g, int& _b, int& _a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	int get_raw() const
	{
		return *((int*)this);
	}

	inline int r() const
	{
		return _color[0];
	}

	inline int g() const
	{
		return _color[1];
	}

	inline int b() const
	{
		return _color[2];
	}

	inline int a() const
	{
		return _color[3];
	}

	inline void set_red(int value)
	{
		_color[0] = value;
	}

	inline void set_green(int value)
	{
		_color[1] = value;
	}

	inline void set_blue(int value)
	{
		_color[2] = value;
	}

	inline void set_alpha(int value)
	{
		_color[3] = value;
	}

	unsigned char& operator[](int index)
	{
		return _color[index];
	}

	const unsigned char& operator[](int index) const
	{
		return _color[index];
	}

	bool operator ==(const color& rhs) const
	{
		return rhs.r() == this->r()
			&& rhs.g() == this->g()
			&& rhs.b() == this->b()
			&& rhs.a() == this->a();
	}

	bool operator !=(const color& rhs) const
	{
		return !(operator==(rhs));
	}

	color& operator=(const color& rhs)
	{
		color clr = rhs;
		set_u32(clr.u32());
		return *this;
	}

	void set_u32(unsigned long color32)
	{
		*((unsigned long*)_color) = color32;
	}

	inline unsigned long u32()
	{
		return *((unsigned long*)_color);
	}

	float get_hue()
	{
		float r = _color[0] / 255.f;
		float g = _color[1] / 255.f;
		float b = _color[2] / 255.f;

		float mx = std::max(r, std::max(g, b));
		float mn = std::min(r, std::min(g, b));
		if (mx == mn)
			return 0.f;

		float delta = mx - mn;

		float hue = 0.f;
		if (mx == r)
			hue = (g - b) / delta;
		else if (mx == g)
			hue = 2.f + (b - r) / delta;
		else
			hue = 4.f + (r - g) / delta;

		hue *= 60.f;
		if (hue < 0.f)
			hue += 360.f;

		return hue / 360.f;
	}

	float get_saturation()
	{
		float r = _color[0] / 255.f;
		float g = _color[1] / 255.f;
		float b = _color[2] / 255.f;

		float mx = std::max(r, std::max(g, b));
		float mn = std::min(r, std::min(g, b));

		float delta = mx - mn;

		if (mx == 0.f)
			return delta;

		return delta / mx;
	}

	float get_brightness()
	{
		float r = _color[0] / 255.f;
		float g = _color[1] / 255.f;
		float b = _color[2] / 255.f;

		return std::max(r, std::max(g, b));
	}

	static color hsb(float hue, float saturation, float brightness)
	{
		hue = std::clamp(hue, 0.f, 1.f);
		saturation = std::clamp(saturation, 0.f, 1.f);
		brightness = std::clamp(brightness, 0.f, 1.f);

		float h = (hue == 1.f) ? 0.f : (hue * 6.f);
		float f = h - static_cast<int>(h);
		float p = brightness * (1.f - saturation);
		float q = brightness * (1.f - saturation * f);
		float t = brightness * (1.f - (saturation * (1.f - f)));

		if (h < 1.f)
			return color((int)(brightness * 255), (int)(t * 255), (int)(p * 255));
		else if (h < 2.f)
			return color((int)(q * 255), (int)(brightness * 255), (int)(p * 255));
		else if (h < 3.f)
			return color((int)(p * 255), (int)(brightness * 255), (int)(t * 255));
		else if (h < 4)
			return color((int)(p * 255), (int)(q * 255), (int)(brightness * 255));
		else if (h < 5)
			return color((int)(t * 255), (int)(p * 255), (int)(brightness * 255));
		else
			return color((int)(brightness * 255), (int)(p * 255), (int)(q * 255));
	}

	color manage_alpha(int alpha)
	{
		return color(_color[0], _color[1], _color[2], alpha);
	}

	vector4d as_vector_4d()
	{
		return vector4d(_color[0] / 255.f, _color[1] / 255.f, _color[2] / 255.f, _color[3] / 255.f);
	}

private:
	unsigned char _color[4];
};