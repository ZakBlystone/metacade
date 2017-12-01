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
vec2.cpp:
===============================================================================
*/

#include "core_private.h"

inline CVec2::CVec2()
{
	//set(0, 0);
}

inline CVec2::CVec2(float x, float y)
{
	set(x, y);
}

inline CVec2::CVec2(const CVec2 &other)
{
	set(other._x, other._y);
}

inline void CVec2::set(const CVec2 &other)
{
	set(other._x, other._y);
}

inline void CVec2::set(float xy[2])
{
	set(xy[0], xy[1]);
}

inline void CVec2::set(float x, float y)
{
	_xy[0] = x; _xy[1] = y;
}

float CVec2::getX() const
{
	return _xy[0];
}

float CVec2::getY() const
{
	return _xy[1];
}

void CVec2::get(float *ptr) const
{
	ptr[0] = _xy[0]; ptr[1] = _xy[1];
}

float CVec2::lengthSquared() const
{
	return dot(*this);
}

float CVec2::length() const
{
	return sqrtf(lengthSquared());
}

float CVec2::distanceSquared(const CVec2 &other) const
{
	CVec2 d = other - *this;
	return d.length();
}

float CVec2::distance(const CVec2 &other) const
{
	return sqrtf(distanceSquared(other));
}

float CVec2::dot(const CVec2 &other) const
{
	return (_xy[0] * other._xy[0]) + (_xy[1] * other._xy[1]);
}

float CVec2::cross(const CVec2 &other) const
{
	return (_xy[0] * other._xy[1]) - (_xy[1] * other._xy[0]);
}

float CVec2::getAngle() const
{
	return atan2(_xy[1], _xy[0]);
}

float CVec2::angleTo(const CVec2 &other) const
{
	CVec2 d = other - *this;
	return d.getAngle();
}

bool CVec2::inRect(const CVec2 &pos, const CVec2 &size) const
{
	if(_xy[0] < pos.getX() || _xy[0] > pos.getX() + size.getX()) return false;
	if(_xy[1] < pos.getY() || _xy[1] > pos.getY() + size.getY()) return false;
	return true;
}

bool CVec2::inBox(const CVec2 &min, const CVec2 &max) const
{
	if(_xy[0] < min.getX() || _xy[0] > max.getX()) return false;
	if(_xy[1] < min.getY() || _xy[1] > max.getY()) return false;
	return true;
}

inline void CVec2::vmin(const CVec2 &b)
{
	_x = min(_x, b._x);
	_y = min(_y, b._y);
}

inline void CVec2::vmax(const CVec2 &b)
{
	_x = max(_x, b._x);
	_y = max(_y, b._y);
}

CVec2& CVec2::operator+=(const CVec2 &other)
{
	_xy[0] += other._xy[0];
	_xy[1] += other._xy[1];
	return *this;
}

CVec2& CVec2::operator-=(const CVec2 &other)
{
	_xy[0] -= other._xy[0];
	_xy[1] -= other._xy[1];
	return *this;
}

CVec2& CVec2::operator*=(const CMatrix3 &other)
{
	*this = *this * other;
	return *this;
}

CVec2& CVec2::operator*=(const CVec2 &other)
{
	_xy[0] *= other._xy[0];
	_xy[1] *= other._xy[1];
	return *this;
}

CVec2& CVec2::operator*=(float scalar)
{
	_xy[0] *= scalar;
	_xy[1] *= scalar;
	return *this;
}

CVec2& CVec2::operator/=(const CVec2 &other)
{
	_xy[0] /= other._xy[0];
	_xy[1] /= other._xy[1];
	return *this;
}

CVec2& CVec2::operator/=(float scalar)
{
	_xy[0] /= scalar;
	_xy[1] /= scalar;
	return *this;
}

CVec2 CVec2::operator+(const CVec2 &other) const
{
	return CVec2(
		_xy[0] + other._xy[0],
		_xy[1] + other._xy[1]);
}

CVec2 CVec2::operator-(const CVec2 &other) const
{
	return CVec2(
		_xy[0] - other._xy[0],
		_xy[1] - other._xy[1]);
}

CVec2 CVec2::operator*(const CMatrix3 &other) const
{
	CVec2 out;
	out._x = _x * other(0,0) + _y * other(0,1) + other(0,2);
	out._y = _x * other(1,0) + _y * other(1,1) + other(1,2);
	return out;
}

CVec2 CVec2::operator*(const CVec2 &other) const
{
	return CVec2(
		_xy[0] * other._xy[0],
		_xy[1] * other._xy[1]);
}

CVec2 CVec2::operator*(float scalar) const
{
	return CVec2(
		_xy[0] * scalar,
		_xy[1] * scalar);
}

CVec2 CVec2::operator/(const CVec2 &other) const
{
	return CVec2(
		_xy[0] / other._xy[0],
		_xy[1] / other._xy[1]);
}

CVec2 CVec2::operator/(float scalar) const
{
	return CVec2(
		_xy[0] / scalar,
		_xy[1] / scalar);
}

bool CVec2::operator==(const CVec2 &other) const
{
	return _xy[0] == other._xy[0] && _xy[1] == other._xy[1];
}

bool CVec2::operator!=(const CVec2 &other) const
{
	return _xy[0] != other._xy[0] || _xy[1] != other._xy[1];
}

CVec2 CVec2::interpolateTo(const CVec2 &other, float fraction) const
{
	return CVec2(
		this->_x + (other._x - this->_x) * fraction,
		this->_y + (other._y - this->_y) * fraction
	);
}

void CVec2::interpolateTo(const CVec2& A, const CVec2& B, CVec2& result, float fraction)
{
	result._x = B._x + (B._x - A._x) * fraction;
	result._y = B._y + (B._y - A._y) * fraction;
}

CVec2& CVec2::normalize()
{
	float len = length();
	_x = _x / len;
	_y = _y / len;
	return (*this);
}

float& CVec2::operator[](int i)
{
	return _xy[i];
}

float CVec2::operator[](int i) const
{
	return _xy[i];
}

CVec2 CVec2::operator--()
{
	return CVec2(-_xy[0], -_xy[1]);
}

CVec2 operator/(float scalar, CVec2 vec)
{
	return CVec2(scalar / vec._x, scalar / vec._y);
}
