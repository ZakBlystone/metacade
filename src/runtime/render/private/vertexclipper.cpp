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
vertexclipper.cpp:
===============================================================================
*/

#include "render_private.h"

EPointClassify CVertexClipper::classifiedPoints[MAX_VERTICES];

/*
EPointClassify CVertexClipper::clipPolygonAgainstPlane(vector<CVertex2D>& list, const CHalfPlane& plane)
{
	static EPointClassify classifiedPoints[MAX_VERTICES];
	float frac;
	uint32 size = (uint32) list.size();
	uint32 i, j;
	uint32 num_behind = 0;
	uint32 num_infront = 0;

	for ( i=0; i<size; ++i )
	{
		classifiedPoints[i] = plane.clasifyPoint(list[i]._position);
		classifiedPoints[i] == PLANE_BEHIND ? ++num_behind : ++num_infront;
	}

	if ( num_behind == 0 ) return PLANE_INFRONT;
	if ( num_infront == 0 ) return PLANE_BEHIND;

	vector<CVertex2D> output;
	output.reserve(list.size());
	for ( i=0, j=1; i<size; ++i, j=(i+1) % size )
	{
		CVertex2D &v0 = list[i];
		CVertex2D &v1 = list[j];

		EPointClassify &p0 = classifiedPoints[i];
		EPointClassify &p1 = classifiedPoints[j];

		if ( p0 == PLANE_INFRONT && p1 == PLANE_INFRONT ) {}
		else if ( p0 == PLANE_INFRONT && p1 == PLANE_BEHIND )
		{
			plane.intersection(v0._position, v1._position, frac);
			output.push_back(v0.interpolateTo(v1, frac));
			output.push_back(v1);
		}
		else if ( p0 == PLANE_BEHIND && p1 == PLANE_INFRONT )
		{
			plane.intersection(v1._position, v0._position, frac);
			output.push_back(v1.interpolateTo(v0, frac));
		}
		else
		{
			output.push_back(v1);
		}
	}

	list.assign(output.begin(), output.end());
	return PLANE_INTERSECT;
}

*/