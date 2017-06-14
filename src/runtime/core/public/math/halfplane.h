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
halfplane.h: Vec3 with plane characteristics
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CHalfPlane : public CVec3
{
public:
	//Constructs a half-plane, values are uninitialized
	inline CHalfPlane();

	//Constructs a half-plane from a direction and a distance
	inline CHalfPlane(const CVec2& dir, float distance);

	//Constructs a half-plane from a direction and a point on the plane (origin)
	inline CHalfPlane(const CVec2& dir, const CVec2& origin);

	//Calculates the distance between the plane and 'point'
	inline float distance(const CVec2& point) const;

	//Calculates the intersection of the line [start, end] and the plane
	//Only returns intersection if start->end faces into the plane from the front
	//TOI is stored in 'fraction', returns type of intersection
	inline EPointClassify intersection(const CVec2& start, const CVec2& end, float& fraction) const;

	//Returns whether or not 'point' is behind or in front of the plane
	//Passing template parameter 'true' for 'CheckOn' will cause this to also return if the point is on the plane
	template<bool CheckOn = false>
	inline EPointClassify classifyPoint(const CVec2& point) const
	{
		if ( distance(point) <= -EPSILON ) return PLANE_BEHIND;
		return PLANE_INFRONT;
	}
};

//Template specialization for 'classifyPoint' that returns it the point is on the plane as well
template<>
inline EPointClassify CHalfPlane::classifyPoint<true>(const CVec2& point) const
{
	float dist = distance(point);
	if ( dist <= -EPSILON ) return PLANE_BEHIND;
	if ( dist <= EPSILON ) return PLANE_ON;
	return PLANE_INFRONT;
}

}