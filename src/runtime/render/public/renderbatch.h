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
renderbatch.h: Indices into render buffer, also has renderstate and primitive
===============================================================================
*/

#pragma once

#include "metacade_types.h"
#include "render/public/renderstate.h"
//#include "core/core_public.h"

class CRenderBatch
{
public:
	CRenderBatch();
	CRenderBatch(uint32 first, uint32 num, CRenderState renderState, EPrimitive primitive);

	uint32 ARCADE_API getFirstIndex() const;
	uint32 ARCADE_API getNumIndices() const;

	const ARCADE_API class CRenderState getRenderState() const;
	const ARCADE_API EPrimitive getPrimitive() const;
	const ARCADE_API uint32 getStateChangeFlags() const;

	void createStateChangeFlags(const CRenderState& previousState);

private:
	uint32 _firstIndex;
	uint32 _numIndices;
	CRenderState _renderState;
	EPrimitive _primitive;
	uint32 _stateChangeFlags;
};