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
renderelement.cpp:
===============================================================================
*/

#include "render_private.h"

CRenderElement::CRenderElement()
	: _type(RET_NONE)
{
	
}

CRenderElement& CRenderElement::makeQuad(const CRenderQuad& quad, const CClipShape& clip, const CRenderState& renderState, int32 layer /*= 0*/)
{
	_type = RET_QUAD;
	_quad = quad;
	_clip = clip;
	_renderState = renderState;
	_layer = layer;
	return *this;
}


CRenderQuad& CRenderElement::makeQuad2(const CClipShape& clip, const CRenderState& state, int32 layer /*= 0*/)
{
	_type = RET_QUAD;
	_clip = clip;
	_renderState = state;
	_layer = layer;
	return _quad;
}

ERenderElementType CRenderElement::getType() const
{
	return _type;
}

const CRenderQuad& CRenderElement::getQuad() const
{
	return _quad;
}

const CRenderState& CRenderElement::getRenderState() const
{
	return _renderState;
}

const CClipShape& CRenderElement::getClipShape() const
{
	return _clip;
}

int32 CRenderElement::getLayer() const
{
	return _layer;
}

CClipShape& CClipShape::operator-=(const CClipShape& other)
{
	//total number of planes to consider
	int32 num = _numPlanes + other._numPlanes;

	//temporary array to combine lists of planes
	CHalfPlane temp[MAX_CLIPPING_PLANES*2];

	//temporary markers to determine which planes are valid
	bool marked[MAX_CLIPPING_PLANES*2] = {false};

	//copy this shape's planes into temp
	memcpy(temp, _planes, _numPlanes * sizeof(CHalfPlane));

	//copy other shape's planes into temp
	memcpy(temp + _numPlanes, other._planes, other._numPlanes * sizeof(CHalfPlane));

	//making a new set of planes
	_numPlanes = 0;

	//temporary for point of intersection
	CVec2 point;

	//iterate over every pair of planes
	for ( int32 i=0; i<num; ++i )
	{
		for ( int32 j=i+1; j<num; ++j )
		{
			//continue until two intersecting planes are found
			if ( !temp[i].intersection(temp[j], point) ) continue;

			//verify that the intersection point is 'behind' all other planes
			bool valid = true;
			for (int32 k = 0; k < num; ++k)
			{
				if (k == i || k == j) continue;
				if (temp[k].distance(point) > EPSILON) { valid = false; break; }
			}

			//mark the two planes as valid
			if (valid)
			{
				marked[i] = true;
				marked[j] = true;
			}
		}

		//if plane is a valid part of the set, add it
		//break once we've reached MAX_CLIPPING_PLANES
		if ( marked[i] && !add(temp[i]) ) break;
	}

	return *this;
}
