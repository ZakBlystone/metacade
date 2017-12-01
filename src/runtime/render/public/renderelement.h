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
renderelement.h:
===============================================================================
*/

#pragma once

#include "metacade_types.h"
#include "core/core_public.h"
#include "render/public/renderstate.h"

namespace Arcade
{

struct CRenderQuad
{
	CVertex2D _verts[4];

	inline void makeBox(const CVec2 &mins, const CVec2 &maxs, const CColor &color)
	{
		_verts[0]._position.set(mins);
		_verts[1]._position.set(maxs._x, mins._y);
		_verts[2]._position.set(maxs);
		_verts[3]._position.set(mins._x, maxs._y);

		_verts[0]._texcoord.set(0,0);
		_verts[1]._texcoord.set(1,0);
		_verts[2]._texcoord.set(1,1);
		_verts[3]._texcoord.set(0,1);
		
		_verts[0]._color = color;
		_verts[1]._color = color;
		_verts[2]._color = color;
		_verts[3]._color = color;
	}

	void transform(const CMatrix3 &matrix)
	{
		_verts[0]._position *= matrix;
		_verts[1]._position *= matrix;
		_verts[2]._position *= matrix;
		_verts[3]._position *= matrix;
	}

	void transformUV(const CMatrix3 &matrix)
	{
		_verts[0]._texcoord *= matrix;
		_verts[1]._texcoord *= matrix;
		_verts[2]._texcoord *= matrix;
		_verts[3]._texcoord *= matrix;
	}
};

#define MAX_CLIPPING_PLANES 16
struct CClipShape
{
public:
	inline CClipShape() 
		: _numPlanes(0)
	{}
	
	inline bool add(const CHalfPlane& plane)
	{
		if ( _numPlanes >= MAX_CLIPPING_PLANES - 1 ) return false;
		_planes[_numPlanes++] = plane;
		return true;
	}

	inline const CHalfPlane& getHalfPlane(int32 i) const { return _planes[i]; }
	inline int32 getNumPlanes() const { return _numPlanes; }

private:
	CHalfPlane _planes[MAX_CLIPPING_PLANES];
	int32 _numPlanes;
};

class CRenderElement
{
public:
	CRenderElement();

	CRenderElement& makeQuad(const CRenderQuad& quad, const CClipShape& clip, const CRenderState& renderState, int32 layer = 0);
	
	CRenderQuad& makeQuad2(const CClipShape& clip, const CRenderState& state, int32 layer = 0);

	ERenderElementType getType() const;
	const CRenderQuad& getQuad() const;
	const CRenderState& getRenderState() const;
	const CClipShape& getClipShape() const;
	int32 getLayer() const;

private:
	ERenderElementType _type;
	CRenderState _renderState;
	int32 _layer;
	CRenderQuad _quad;
	CClipShape _clip;
};

}