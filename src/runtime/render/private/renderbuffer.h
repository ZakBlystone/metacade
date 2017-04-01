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
renderbuffer.h: VBO / IBO storage on CPU
===============================================================================
*/

#pragma once

#include "metacade_types.h"
#include "core/core_public.h"

namespace Arcade
{

class CRenderBuffer : public IRenderBuffer
{
public:
	CRenderBuffer();

	inline void addVertex(const CVertex2D &vertex)
	{
		if ( _numVerts >= MAX_VERTICES ) return;
		_svert[_numVerts++] = vertex;
	}

	inline void addIndex(const uint16 index = 0)
	{
		if ( _numIndices >= MAX_INDICES ) return;
		_sindex[_numIndices++] = index;
	}

	virtual const CVertex2D* getVertices() const;
	virtual const uint16 getNumVertices() const;

	virtual const uint16* getIndices() const;
	virtual const uint32 getNumIndices() const;

	void setNumVertices(uint32 num);

	void clear();

private:

	CVertex2D _svert[MAX_VERTICES];
	uint16 _sindex[MAX_INDICES];

	uint32 _numVerts;
	uint32 _numIndices;
};

}