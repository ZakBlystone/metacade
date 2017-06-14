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
color.h: 24-bit color representation (w/alpha) and floating-point counterpart
===============================================================================
*/

#pragma once

namespace Arcade
{

struct CFloatColor;

//24-bit color represented in little endian RGBA format [ 0xRRGGBBAA ]
struct METACADE_API CColor
{
	union
	{
		struct
		{
			uint8 r,g,b,a;
		};
		uint8 rgba[4];
		uint32 irgba;
	};

	//Construct an empty CColor object, initializes as opaque black (0x000000FF)
	inline CColor();

	//Construct from an RGBA packed integer
	inline CColor(uint32 irgba);

	//Construct from an 8-bit RGBA array
	inline CColor(uint8 color[4]);

	//Construct from four 8-bit values (alpha defaults to opaque)
	inline CColor(uint8 cr, uint8 cg, uint8 cb, uint8 ca = 0xFF);

	//Converts CColor to a packed RGBA integer
	uint32 asInt() const;

	/*friend ostream& operator << (ostream& os, const CColor& v)
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
	}*/
};

//color represented as floating point values per channel
struct METACADE_API CFloatColor
{
	union
	{
		struct
		{
			float r,g,b,a;
		};
		float rgba[4];
		uint32 iRGBA;
	};

	//Construct an empty CFloatColor object, initializes as opaque black (0x000000FF)
	inline CFloatColor();

	//Construct from a 24-bit CColor
	inline CFloatColor(const CColor &color);

	//Construct from four floating point values (alpha defaults to opaque)
	inline CFloatColor(float fr, float fg, float fb, float fa = 1.0f);

	//Arithmetic operators
	CFloatColor operator+(const CFloatColor& other) const;
	CFloatColor operator+(float brt) const;
	CFloatColor operator-(const CFloatColor& other) const;
	CFloatColor operator-(float brt) const;
	CFloatColor operator*(float frac) const;

	CFloatColor& operator+=(const CFloatColor& other);
	CFloatColor& operator+=(float brt);
	CFloatColor& operator-=(const CFloatColor& other);
	CFloatColor& operator-=(float brt);

	//Interpolates between two colors
	inline CFloatColor interpolateTo(const CFloatColor& other, float fraction) const;

	//Conversion operator to convert to 24-bit CColor
	operator CColor() const;
};

}