/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
color.cpp: 32-bit color representation and floating-point counterpart
===============================================================================
*/

#include "core_private.h"

//CColor
inline CColor::CColor(uint8 cr, uint8 cg, uint8 cb, uint8 ca /*= 0xFF*/) : _r(cr), _g(cg), _b(cb), _a(ca)
{

}

inline CColor::CColor(uint8 color[4]) : _r(color[0]), _g(color[1]), _b(color[2]), _a(color[3])
{

}

inline CColor::CColor(uint32 irgba)
{
	//Unpack RGBA from uint32 into the [r,g,b,a] union
	_r = (irgba >> 24) & 0xFF;
	_g = (irgba >> 16) & 0xFF;
	_b = (irgba >> 8) & 0xFF;
	_a = (irgba)& 0xFF;
}

inline CColor::CColor() : _r(0), _g(0), _b(0), _a(0xFF)
{

}

uint32 CColor::asInt() const
{
	//Pack RGBA from the [r,g,b,a] union
	return (_r << 24) | (_g << 16) | (_b << 8) | _a;
}

//CFloatColor
inline CFloatColor::CFloatColor() : r(0.f), g(0.f), b(0.f), a(1.f)
{

}

inline CFloatColor::CFloatColor(const CColor& color)
{
	//Divide out maximum unsigned 8-bit color value from each channel, cast to float
	static const float scale = 1.0f / 255.f;
	r = (float)(color._r) * scale;
	g = (float)(color._g) * scale;
	b = (float)(color._b) * scale;
	a = (float)(color._a) * scale;
}

inline CFloatColor::CFloatColor(float fr, float fg, float fb, float fa /*= 1.0f*/)
{
	//Clamp input parameters and copy them into the [r,g,b,a] union
	r = fr > 1.0f ? 1.0f : (fr < 0.0f ? 0.0f : fr);
	g = fg > 1.0f ? 1.0f : (fg < 0.0f ? 0.0f : fg);
	b = fb > 1.0f ? 1.0f : (fb < 0.0f ? 0.0f : fb);
	a = fa > 1.0f ? 1.0f : (fa < 0.0f ? 0.0f : fa);
}

CFloatColor CFloatColor::operator-(const CFloatColor& other) const
{
	//Per-channel subtraction ( this[x] - other[x] )
	const float *fl0 = this->rgba;
	const float *fl1 = other.rgba;
	return CFloatColor(fl0[0] - fl1[0], fl0[1] - fl1[1], fl0[2] - fl1[2], fl0[3] - fl1[3]);
}

CFloatColor CFloatColor::operator-(float brt) const
{
	//Subtract 'brt' from each channel ( this[x] - brt )
	const float *fl0 = this->rgba;
	return CFloatColor(fl0[0] - brt, fl0[1] - brt, fl0[2] - brt, fl0[3] - brt);
}

CFloatColor CFloatColor::operator+(const CFloatColor& other) const
{
	//Per-channel addition ( this[x] + other[x] )
	const float *fl0 = this->rgba;
	const float *fl1 = other.rgba;
	return CFloatColor(fl0[0] + fl1[0], fl0[1] + fl1[1], fl0[2] + fl1[2], fl0[3] + fl1[3]);
}

CFloatColor CFloatColor::operator+(float brt) const
{
	//Subtract 'brt' to each channel ( this[x] + brt )
	const float *fl0 = this->rgba;
	return CFloatColor(fl0[0] + brt, fl0[1] + brt, fl0[2] + brt, fl0[3] + brt);
}

CFloatColor CFloatColor::operator*(float frac) const
{
	//Scale each channel by 'frac' ( this[x] * frac )
	const float *fl0 = this->rgba;
	return CFloatColor(fl0[0] * frac, fl0[1] * frac, fl0[2] * frac, fl0[3] * frac);
}

CFloatColor& CFloatColor::operator+=(const CFloatColor& other)
{
	(*this) = (*this) + other;
	return *this;
}

CFloatColor& CFloatColor::operator+=(float brt)
{
	(*this) = (*this) + brt;
	return *this;
}

CFloatColor& CFloatColor::operator-=(const CFloatColor& other)
{
	(*this) = (*this) - other;
	return *this;
}

CFloatColor& CFloatColor::operator-=(float brt)
{
	(*this) = (*this) - brt;
	return *this;
}

CFloatColor CFloatColor::interpolateTo(const CFloatColor& other, float fraction) const
{
	//Linear interpolation between 'this' and 'other'
	const float *fl0 = this->rgba;
	const float *fl1 = other.rgba;
	return CFloatColor(
		fl0[0] + (fl1[0] - fl0[0]) * fraction,
		fl0[1] + (fl1[1] - fl0[1]) * fraction,
		fl0[2] + (fl1[2] - fl0[2]) * fraction,
		fl0[3] + (fl1[3] - fl0[3]) * fraction);
}

CFloatColor::operator CColor() const
{
	//Multiply each channel by maximum unsigned 8-bit color value
	return CColor(
		(unsigned char)(r * 255.0f),
		(unsigned char)(g * 255.0f),
		(unsigned char)(b * 255.0f),
		(unsigned char)(a * 255.0f)
	);
}
