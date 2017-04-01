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
	: _numVerts(0)
	, _numIndices(0)
{

}

CVertex2D& CRenderBuffer::addVertex(const CVertex2D &vertex)
{
	static CVertex2D novertex;
	if ( _numVerts >= MAX_VERTICES ) return novertex;

	//memcpy(_svert + _numVerts, &vertex, sizeof(CVertex2D));
	_svert[_numVerts] = vertex;
	return _svert[_numVerts++];
}

uint16& CRenderBuffer::addIndex(const uint16 index /*= 0*/)
{
	static uint16 noindex = 0;
	if ( _numIndices >= MAX_INDICES ) return noindex;

	//memcpy(_sindex + _numIndices, &index, sizeof(uint16));
	_sindex[_numIndices] = index;
	return _sindex[_numIndices++];
}

const CVertex2D* CRenderBuffer::getVertices() const
{
	return _svert;
}

const uint16 CRenderBuffer::getNumVertices() const
{
	return _numVerts;
}

const uint16* CRenderBuffer::getIndices() const
{
	return _sindex;
}

const uint32 CRenderBuffer::getNumIndices() const
{
	return _numIndices;
}

void CRenderBuffer::setNumVertices(uint32 num)
{
	_numVerts = num;
}

void CRenderBuffer::clear()
{
	_numVerts = 0;
	_numIndices = 0;
}