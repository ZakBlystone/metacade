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

CHalfPlane::CHalfPlane(const CVec2& dir, float distance)
	: CVec3(dir.x, dir.y, distance)
{

}

float CHalfPlane::distance(const CVec2& point) const
{
	return (point.x * this->x + point.y * this->y) - this->z;
}

EPointClassify CHalfPlane::intersection(const CVec2& start, const CVec2& end, float& fraction)
{
	float d1 = distance(start);
	float d2 = distance(end);
	float dist = fabs(d1) + fabs(d2);

	if ( d1 > EPSILON && d2 < -EPSILON )
	{
		fraction = (d1 / dist);
		return PLANE_INTERSECT;
	}

	fraction = 0.f;

	if ( d1 <= -EPSILON || d2 <= EPSILON )
	{
		return PLANE_BEHIND;
	}

	return PLANE_INFRONT;
}

EPointClassify CHalfPlane::clasifyPoint(const CVec2& point) const
{
	float dist = distance(point);
	if ( dist <= -EPSILON ) return PLANE_BEHIND;
	if ( dist <= EPSILON ) return PLANE_ON;
	return PLANE_INFRONT;
}
