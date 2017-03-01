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

#include "metacade_private.h"
#include "core/core_public.h"

class CRenderBuffer
{
public:
	CRenderBuffer();

	CVertex2D& addVertex(const CVertex2D &vertex);
	uint32& addIndex(const uint32 index = 0);

	const RUNTIME_API CVertex2D* getVertices() const;
	const RUNTIME_API uint32 getNumVertices() const;

	const RUNTIME_API uint32* getIndices() const;
	const RUNTIME_API uint32 getNumIndices() const;

	void clear();

private:

	vector<CVertex2D> _vertices;
	vector<uint32> _indices;
};
