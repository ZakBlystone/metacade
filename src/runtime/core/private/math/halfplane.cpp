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
halfplane.cpp: Vec3 with plane characteristics
===============================================================================
*/

#include "core_private.h"

inline CHalfPlane::CHalfPlane(const CVec2& dir, float distance)
	: CVec3(dir._x, dir._y, distance)
{

}

inline CHalfPlane::CHalfPlane(const CVec2& dir, const CVec2& origin)
	: CVec3(dir._x, dir._y, origin.dot(dir))
{

}

inline CHalfPlane::CHalfPlane()
{

}

inline float CHalfPlane::distance(const CVec2& point) const
{
	//dot(point, normal) - distance
	return (point._x * this->_x + point._y * this->_y) - this->_z;
}

inline EPointClassify CHalfPlane::intersection(const CVec2& start, const CVec2& end, float& fraction) const
{
	//Calculate the distance to each point of the segment formed by [start, end]
	float d1 = distance(start);
	float d2 = distance(end);

	//'end' is behind the plane, 'start' is in front of the plane
	if ( d1 > EPSILON && d2 < -EPSILON )
	{
		fraction = (d1 / (d1 - d2));
		return PLANE_INTERSECT;
	}

	fraction = 0.f;

	//both points are behind the plane, no intersection
	if ( d1 <= -EPSILON || d2 <= EPSILON )
	{
		return PLANE_BEHIND;
	}

	return PLANE_INFRONT;
}

bool CHalfPlane::intersection(const CHalfPlane& other, CVec2& out) const
{
	//origin of other plane
	float otherX = other._x * other._z;
	float otherY = other._y * other._z;

	//dotProduct(p0 - p1, normal)
	float a = (_x * _z - otherX) * _x + (_y * _z - otherY) * _y;

	//dot product between this normal and perpendicular normal of other plane
	float b = other._y * _x - other._x * _y;

	//check if parallel
	if ( b == 0.f ) return false;

	//projection along vector perpendicular to other plane
	float c = a / b;
	out.set(otherX + other._y * c, otherY - other._x * c);

	return true;
}
