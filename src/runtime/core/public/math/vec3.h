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
vec3.h:
===============================================================================
*/

#pragma once

#include "metacade_private.h"

namespace Arcade
{

class CMatrix4;
class ARCADE_API CVec3 
{
public:
	union
	{
		struct
		{
			float x,y,z;
		};
		float _xyz[3];
	};

	//Constructor
	CVec3();
	CVec3(float s);
	CVec3(float x, float y, float z);
	CVec3(const CVec3 &other);

	//Accessors and Mutators
	inline void set(float vx, float vy, float vz);
	inline void set(float xyz[3]);
	inline void set(const CVec3 &other);
	inline float getX() const;
	inline float getY() const;
	inline float getZ() const;
	inline void get(float *ptr) const;

	//Utility Functions
	bool equal(const CVec3 &other) const;
	float lengthSquared() const;
	float length() const;
	float distanceSquared(const CVec3 &other) const;
	float distance(const CVec3 &other) const;
	float dot(const CVec3 &other) const;
	CVec3 perpendicular() const;
	CVec3 project(CVec3 normal) const;
	CVec3 cross(const CVec3 &other) const;
	CVec3 getNormal() const;
	CVec3& normalize();
	CVec3 interpolateTo(const CVec3 &other, float fraction) const;

	//Operator Overloads
	/*friend ostream& operator << (ostream &os, const CVec3 &v)
	{
		os << "[";
		os << v.x << ", ";
		os << v.y << ", ";
		os << v.z << "]";
		return os;
	}

	friend istream& operator >> (istream &is, CVec3 &v)
	{
		is >> v.x;
		is >> v.y;
		is >> v.z;
		return is;
	}*/

	float operator[](int i) const;
	float& operator[](int i);

	CVec3 operator--();
	CVec3& operator+=(const CVec3 &other);
	CVec3& operator-=(const CVec3 &other);
	CVec3& operator*=(const CVec3 &other);
	CVec3& operator*=(const CMatrix4 &other);
	CVec3& operator*=(float scalar);
	CVec3& operator/=(const CVec3 &other);
	CVec3& operator/=(float scalar);

	CVec3 operator+(const CVec3 &other) const;
	CVec3 operator-(const CVec3 &other) const;
	CVec3 operator*(const CVec3 &other) const;
	CVec3 operator*(const CMatrix4 &other) const;
	CVec3 operator*(float scalar) const;
	CVec3 operator/(const CVec3 &other) const;
	CVec3 operator/(float scalar) const;
};

static const CVec3 Vec3Zero(0,0,0);

}