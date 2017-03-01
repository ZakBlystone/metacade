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

#include "metacade_private.h"

class RUNTIME_API CHalfPlane : public CVec3
{
	CHalfPlane(const CVec2& dir, float distance);

	float distance(const CVec2& point) const;
	EPointClassify intersection(const CVec2& start, const CVec2& end, float& fraction);
	EPointClassify clasifyPoint(const CVec2& point) const;
};
