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
renderelement.cpp:
===============================================================================
*/

#include "render_private.h"

CRenderElement::CRenderElement()
	: _type(RET_NONE)
{
	
}

CRenderElement& CRenderElement::makeQuad(const CRenderQuad& quad, const CClipShape& clip, const CRenderState& renderState, int32 layer /*= 0*/)
{
	_type = RET_QUAD;
	_quad = quad;
	_clip = clip;
	_renderState = renderState;
	_layer = layer;
	return *this;
}

ERenderElementType CRenderElement::getType() const
{
	return _type;
}

const CRenderQuad& CRenderElement::getQuad() const
{
	return _quad;
}

const CRenderState& CRenderElement::getRenderState() const
{
	return _renderState;
}

const CClipShape& CRenderElement::getClipShape() const
{
	return _clip;
}

int32 CRenderElement::getLayer() const
{
	return _layer;
}
