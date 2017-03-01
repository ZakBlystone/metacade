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
renderbuffer.cpp:
===============================================================================
*/

#include "render_private.h"

CRenderBuffer::CRenderBuffer()
{
	_vertices.reserve(8192);
	_indices.reserve(16384);
}

CVertex2D& CRenderBuffer::addVertex(const CVertex2D &vertex)
{
	_vertices.emplace_back(vertex); return _vertices.back();
}

uint16& CRenderBuffer::addIndex(const uint16 index /*= 0*/)
{
	_indices.emplace_back(index); return _indices.back();
}

const CVertex2D* CRenderBuffer::getVertices() const
{
	return _vertices.data();
}

const uint16 CRenderBuffer::getNumVertices() const
{
	return (uint16)_vertices.size();
}

const uint16* CRenderBuffer::getIndices() const
{
	return _indices.data();
}

const uint32 CRenderBuffer::getNumIndices() const
{
	return (uint32)_indices.size();
}

void CRenderBuffer::clear()
{
	_vertices.clear();
	_indices.clear();
}
