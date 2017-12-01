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
vec3.cpp:
===============================================================================
*/

#include "core_private.h"

float CVec3::lengthSquared() const
{
	return _x*_x + _y*_y + _z*_z;
}

float CVec3::length() const
{
	return sqrtf(lengthSquared());
}

float CVec3::distanceSquared(const CVec3 &other) const
{
	CVec3 d = other - *this;
	return d.lengthSquared();
}

float CVec3::distance(const CVec3 &other) const
{
	return sqrtf(distanceSquared(other));
}

float CVec3::dot(const CVec3 &other) const
{
	return (_x * other._x) + 
		   (_y * other._y) + 
		   (_z * other._z);
}

CVec3 CVec3::cross(const CVec3 &other) const
{
	return CVec3(
		(_y * other._z) - (_z * other._y),
		(_z * other._x) - (_x * other._z),
		(_x * other._y) - (_y * other._x));
}

CVec3  CVec3::getNormal() const
{
	return CVec3(*this) /= this->length();
}

CVec3 &CVec3::normalize()
{
	return (*this) /= this->length();
}

CVec3 CVec3::interpolateTo(const CVec3 &other, float fraction) const
{
	static CVec3 dir;
	dir.set(other);
	dir -= *(this);
	return (*this) + dir * fraction;
}

inline CVec3::CVec3(const CVec3 &other)
{
	set(other._x, other._y, other._z);
}

inline CVec3::CVec3(float x, float y, float z)
{
	set(x, y, z);
}

inline CVec3::CVec3(float s)
{
	set(s, s, s);
}

inline CVec3::CVec3()
{
	//set(0, 0, 0);
}

inline void CVec3::set(const CVec3 &other)
{
	set(other._x, other._y, other._z);
}

inline void CVec3::set(float xyz[3])
{
	set(xyz[0], xyz[1], xyz[2]);
}

inline void CVec3::set(float vx, float vy, float vz)
{
	_x = vx; _y = vy; _z = vz;
}

inline float CVec3::getX() const
{
	return _x;
}

inline float CVec3::getY() const
{
	return _y;
}

inline float CVec3::getZ() const
{
	return _z;
}

void CVec3::get(float *ptr) const
{
	ptr[0] = _x; ptr[1] = _y; ptr[2] = _z;
}

bool CVec3::equal(const CVec3 &other) const
{
	return abs(_x - other._x) < EPSILON && abs(_y - other._y) < EPSILON && abs(_z - other._z) < EPSILON;
}

CVec3 CVec3::perpendicular() const
{
	int pos = 0;
	int i;
	double minelem = 1;
	CVec3 temp(0,0,0);

	for ( i = 0; i < 3; i++ )
	{
		double d = abs( _xyz[i] );
		if ( d < minelem )
		{
			pos = i;
			minelem = d;
		}
	}

	temp._xyz[pos] = 1.0f;
	return temp.project((*this)).normalize();
}

CVec3 CVec3::project(CVec3 normal) const
{
	float inv_denom = 1.0f / normal.dot(normal);
	float d = normal.dot((*this)) * inv_denom;

	CVec3 vec = normal * inv_denom;
	return (*this) - vec * d;
}

float& CVec3::operator[](int i)
{
	return _xyz[i];
}

float CVec3::operator[](int i) const
{
	return _xyz[i];
}

CVec3 CVec3::operator--()
{
	return CVec3(-_xyz[0], -_xyz[1], -_xyz[2]);
}

CVec3 & CVec3::operator+=(const CVec3 &other)
{
	_x += other._x;
	_y += other._y;
	_z += other._z;
	return *this;
}

CVec3 & CVec3::operator-=(const CVec3 &other)
{
	_x -= other._x;
	_y -= other._y;
	_z -= other._z;
	return *this;
}

CVec3 & CVec3::operator*=(const CVec3 &other)
{
	_x *= other._x;
	_y *= other._y;
	_z *= other._z;
	return *this;
}

CVec3 &CVec3::operator*=(const CMatrix4 &other)
{
	*this = *this * other;
	return *this;
}

CVec3 & CVec3::operator*=(float scalar)
{
	_x *= scalar;
	_y *= scalar;
	_z *= scalar;
	return *this;
}

CVec3 & CVec3::operator/=(const CVec3 &other)
{
	_x /= other._x;
	_y /= other._y;
	_z /= other._z;
	return *this;
}

CVec3 & CVec3::operator/=(float scalar)
{
	_x /= scalar;
	_y /= scalar;
	_z /= scalar;
	return *this;
}

CVec3 CVec3::operator+(const CVec3 &other) const
{
	return CVec3(
		_x + other._x,
		_y + other._y,
		_z + other._z);
}

CVec3 CVec3::operator-(const CVec3 &other) const
{
	return CVec3(
		_x - other._x,
		_y - other._y,
		_z - other._z);
}

CVec3 CVec3::operator*(const CVec3 &other) const
{
	return CVec3(
		_x * other._x,
		_y * other._y,
		_z * other._z);
}

CVec3 CVec3::operator*(const CMatrix4 &other) const
{
    int i,j;
	CVec3 out;
    for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			if(i < 3) out._xyz[i] += _xyz[j] * other(j,i);
		}
    }
	return out;
}

CVec3 CVec3::operator*(float scalar) const
{
	return CVec3(
		_x * scalar,
		_y * scalar,
		_z * scalar);
}

CVec3 CVec3::operator/(const CVec3 &other) const
{
	return CVec3(
		_x / other._x,
		_y / other._y,
		_z / other._z);
}

CVec3 CVec3::operator/(float scalar) const
{
	return CVec3(
		_x / scalar,
		_y / scalar,
		_z / scalar);
}