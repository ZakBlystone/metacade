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

class CRenderBuffer : public IRenderBuffer
{
public:
	CRenderBuffer();

	CVertex2D& addVertex(const CVertex2D &vertex);
	uint16& addIndex(const uint16 index = 0);

	virtual const CVertex2D* getVertices() const;
	virtual const uint16 getNumVertices() const;

	virtual const uint16* getIndices() const;
	virtual const uint32 getNumIndices() const;

	void clear();

private:

	vector<CVertex2D> _vertices;
	vector<uint16> _indices;
};
