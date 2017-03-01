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
	return x*x + y*y + z*z;
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
	return (x * other.x) + 
		   (y * other.y) + 
		   (z * other.z);
}

CVec3 CVec3::cross(const CVec3 &other) const
{
	return CVec3(
		(y * other.z) - (z * other.y),
		(z * other.x) - (x * other.z),
		(x * other.y) - (y * other.x));
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

CVec3::CVec3(const CVec3 &other)
{
	set(other.x, other.y, other.z);
}

CVec3::CVec3(float x, float y, float z)
{
	set(x, y, z);
}

CVec3::CVec3(float s)
{
	set(s, s, s);
}

CVec3::CVec3()
{
	set(0, 0, 0);
}

void CVec3::set(const CVec3 &other)
{
	set(other.x, other.y, other.z);
}

void CVec3::set(float xyz[3])
{
	set(xyz[0], xyz[1], xyz[2]);
}

void CVec3::set(float vx, float vy, float vz)
{
	x = vx; y = vy; z = vz;
}

float CVec3::getX() const
{
	return x;
}

float CVec3::getY() const
{
	return y;
}

float CVec3::getZ() const
{
	return z;
}

void CVec3::get(float *ptr) const
{
	ptr[0] = x; ptr[1] = y; ptr[2] = z;
}

bool CVec3::equal(const CVec3 &other) const
{
	return abs(x - other.x) < EPSILON && abs(y - other.y) < EPSILON && abs(z - other.z) < EPSILON;
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
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

CVec3 & CVec3::operator-=(const CVec3 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

CVec3 & CVec3::operator*=(const CVec3 &other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this;
}

CVec3 &CVec3::operator*=(const CMatrix4 &other)
{
	*this = *this * other;
	return *this;
}

CVec3 & CVec3::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

CVec3 & CVec3::operator/=(const CVec3 &other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

CVec3 & CVec3::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

CVec3 CVec3::operator+(const CVec3 &other) const
{
	return CVec3(
		x + other.x,
		y + other.y,
		z + other.z);
}

CVec3 CVec3::operator-(const CVec3 &other) const
{
	return CVec3(
		x - other.x,
		y - other.y,
		z - other.z);
}

CVec3 CVec3::operator*(const CVec3 &other) const
{
	return CVec3(
		x * other.x,
		y * other.y,
		z * other.z);
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
		x * scalar,
		y * scalar,
		z * scalar);
}

CVec3 CVec3::operator/(const CVec3 &other) const
{
	return CVec3(
		x / other.x,
		y / other.y,
		z / other.z);
}

CVec3 CVec3::operator/(float scalar) const
{
	return CVec3(
		x / scalar,
		y / scalar,
		z / scalar);
}