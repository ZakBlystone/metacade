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
vertexclipper.h:
===============================================================================
*/

#pragma once

#include "metacade_private.h"
#include "core/core_public.h"

namespace Arcade
{

class CVertexClipper
{
public:
	//static EPointClassify clipPolygonAgainstPlane(vector<CVertex2D>& list, const CHalfPlane& plane);
	static EPointClassify classifiedPoints[MAX_VERTICES];

	template<typename EmitFunc>
	static EPointClassify clipPolygonAgainstPlane(const CHalfPlane& plane, const CVertex2D* verts, uint32 numVerts, EmitFunc& callback)
	{
		float frac;
		uint32 i, j;
		uint32 num_behind = 0;
		uint32 num_infront = 0;

		for ( i=0; i<numVerts; ++i )
		{
			classifiedPoints[i] = plane.clasifyPoint(verts[i]._position);
			classifiedPoints[i] == PLANE_BEHIND ? ++num_behind : ++num_infront;
		}

		if ( num_behind == 0 ) return PLANE_INFRONT;
		if ( num_infront == 0 ) return PLANE_BEHIND;

		for ( i=0, j=1; i<numVerts; ++i, j=(i+1) % numVerts )
		{
			const CVertex2D &v0 = verts[i];
			const CVertex2D &v1 = verts[j];

			EPointClassify &p0 = classifiedPoints[i];
			EPointClassify &p1 = classifiedPoints[j];

			if ( p0 == PLANE_INFRONT && p1 == PLANE_INFRONT ) {}
			else if ( p0 == PLANE_INFRONT && p1 == PLANE_BEHIND )
			{
				plane.intersection(v0._position, v1._position, frac);
				callback(v0.interpolateTo(v1, frac));
				callback(v1);
			}
			else if ( p0 == PLANE_BEHIND && p1 == PLANE_INFRONT )
			{
				plane.intersection(v1._position, v0._position, frac);
				callback(v1.interpolateTo(v0, frac));
			}
			else
			{
				callback(v1);
			}
		}

		return PLANE_INTERSECT;
	}
};

}