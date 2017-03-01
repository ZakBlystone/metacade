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
color.h: 32-bit color representation and floating-point counterpart
===============================================================================
*/

#include "metacade_private.h"

#pragma once

struct CFloatColor;
struct RUNTIME_API CColor
{
	union
	{
		struct
		{
			uint8 r,g,b,a;
		};
		uint8 rgba[4];
	};

	CColor();
	CColor(uint32 irgba);

	CColor(uint8 color[4]);
	CColor(uint8 cr, uint8 cg, uint8 cb, uint8 ca = 0xFF);
	CColor(float fr, float fg, float fb, float fa = 1.0f);

	uint32 asInt() const;

	friend ostream& operator << (ostream& os, const CColor& v)
	{
		os << "[";
		os << (unsigned int)(v.r) << ", ";
		os << (unsigned int)(v.g) << ", ";
		os << (unsigned int)(v.b) << ", ";
		os << (unsigned int)(v.a)<< "]";
		return os;
	}

	friend istream& operator >> (istream& is, CColor& v)
	{
		is >> v.r;
		is >> v.g;
		is >> v.b;
		is >> v.a;
		return is;
	}
};

struct RUNTIME_API CFloatColor
{
	union
	{
		struct
		{
			float r,g,b,a;
		};
		float rgba[4];
	};

	CFloatColor(const CColor &color);
	CFloatColor(float fr, float fg, float fb, float fa = 1.0f);

	CFloatColor operator+(const CFloatColor& other) const;
	CFloatColor operator+(float brt) const;
	CFloatColor operator-(const CFloatColor& other) const;
	CFloatColor operator-(float brt) const;
	CFloatColor operator*(float frac) const;

	CFloatColor& operator+=(const CFloatColor& other);
	CFloatColor& operator+=(float brt);
	CFloatColor& operator-=(const CFloatColor& other);
	CFloatColor& operator-=(float brt);

	CFloatColor interpolateTo(const CFloatColor& other, float fraction) const;

	operator CColor() const;
};