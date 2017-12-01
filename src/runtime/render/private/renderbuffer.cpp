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

const CVertex2D* CRenderBuffer::getVertices() const
{
	return _svert;
}

const uint16 CRenderBuffer::getNumVertices() const
{
	return (int16) _numVerts;
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