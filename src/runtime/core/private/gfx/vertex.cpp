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
vertex.cpp: Vertex2D and Vertex3D
===============================================================================
*/

#include "core_private.h"

CVertex3D::CVertex3D() 
	: _color(0xFFFFFFFF)
{

}

CVertex3D::CVertex3D(const CVec3 &pos, const CVec3 &norm, const CVec2 &tc, const CColor &col /*= CColor(0xFFFFFFFF)*/) 
	: _position(pos)
	, _normal(norm)
	, _texcoord(tc)
	, _color(col)
{

}

CVertex3D::CVertex3D(float x, float y, float z, float nx, float ny, float nz, float s, float t, const CColor &col /*= CColor(0xFFFFFFFF) */) 
	: _position(x, y, z)
	, _normal(nx, ny, nz)
	, _texcoord(s, t)
	, _color(col)
{

}

CVertex3D CVertex3D::interpolateTo(const CVertex3D &other, float fraction) const
{
	return CVertex3D(
		_position.interpolateTo(other._position, fraction),
		_normal,
		_texcoord.interpolateTo(other._texcoord, fraction),
		CFloatColor(_color).interpolateTo(other._color, fraction));
}

inline CVertex2D::CVertex2D()
{

}

inline CVertex2D::CVertex2D(const CVec2 &pos, const CVec2 &tc, const CColor &col /*= CColor(0x000000FF)*/) 
	: _position(pos)
	, _texcoord(tc)
	, _color(col)
{

}

inline CVertex2D::CVertex2D(float x, float y, float s, float t, const CColor &col /*= CColor(0xFFFFFFFF)*/) 
	: _position(x, y)
	, _texcoord(s, t)
	, _color(col)
{

}

inline CVertex2D::CVertex2D(const CVertex3D &other) 
	: _position(other._position._x, other._position._y)
	, _texcoord(other._texcoord)
	, _color(other._color)
{

}

CVertex2D CVertex2D::interpolateTo(const CVertex2D &other, float fraction) const
{
	return CVertex2D(
		_position.interpolateTo(other._position, fraction),
		_texcoord.interpolateTo(other._texcoord, fraction),
		CFloatColor(_color).interpolateTo(other._color, fraction));
}

void CVertex2D::interpolateTo(const CVertex2D& A, const CVertex2D& B, CVertex2D& result, float fraction)
{
	result._position._x = A._position._x + (B._position._x - A._position._x) * fraction;
	result._position._y = A._position._y + (B._position._y - A._position._y) * fraction;

	result._texcoord._x = A._texcoord._x + (B._texcoord._x - A._texcoord._x) * fraction;
	result._texcoord._y = A._texcoord._y + (B._texcoord._y - A._texcoord._y) * fraction;

	result._color = CFloatColor(A._color).interpolateTo(B._color, fraction);
}

CVertex3D CVertex2D::to3D() const
{
	return CVertex3D(
		_position._x, //x
		_position._y, //y
		1.0f, //z
		0.0f, //nx
		0.0f, //ny
		1.0f, //nz
		_texcoord._x, //s 
		_texcoord._y, //t
		_color); //color
}