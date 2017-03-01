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
renderbatch.cpp:
===============================================================================
*/

#include "render_private.h"

CRenderBatch::CRenderBatch()
	: _firstIndex(0)
	, _numIndices(0)
{

}

CRenderBatch::CRenderBatch(uint32 first, uint32 num, CRenderState renderState, EPrimitive primitive)
	: _firstIndex(first)
	, _numIndices(num)
	, _renderState(renderState)
	, _primitive(primitive)
{

}

uint32 RUNTIME_API CRenderBatch::getFirstIndex() const
{
	return _firstIndex;
}

uint32 RUNTIME_API CRenderBatch::getNumIndices() const
{
	return _numIndices;
}

const RUNTIME_API class CRenderState CRenderBatch::getRenderState() const
{
	return _renderState;
}

const RUNTIME_API EPrimitive CRenderBatch::getPrimitive() const
{
	return _primitive;
}

const RUNTIME_API uint32 CRenderBatch::getStateChangeFlags() const
{
	return _stateChangeFlags;
}

void CRenderBatch::createStateChangeFlags(const CRenderState& previousState)
{
	_stateChangeFlags = previousState.getStateChangeFlags(_renderState);
}