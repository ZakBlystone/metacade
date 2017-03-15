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
drawbuffer.cpp:
===============================================================================
*/

#include "render_private.h"

CDrawBuffer::CDrawBuffer(shared_ptr<CRenderBuffer> renderBuffer)
	: _renderBuffer(renderBuffer)
{

}

void CDrawBuffer::addRenderBatch(CRenderBatch batch)
{
	_renderBatches.emplace_back(batch);
}

void CDrawBuffer::clearRenderBatches()
{
	_renderBatches.clear();
}

const CRenderBatch* CDrawBuffer::getRenderBatches() const
{
	return _renderBatches.data();
}

uint32 CDrawBuffer::getNumRenderBatches() const
{
	return (uint32) _renderBatches.size();
}

const IRenderBuffer* CDrawBuffer::getRenderBuffer() const
{
	return _renderBuffer.get();
}
