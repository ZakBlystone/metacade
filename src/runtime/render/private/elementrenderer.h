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
elementrenderer.h:
===============================================================================
*/

#pragma once

#include "metacade_private.h"
#include "private/renderlist.h"

namespace Arcade
{

class CElementRenderer
{
public:
	CElementRenderer();

	void beginFrame();

	CRenderElement& addRenderElement();
	void setViewportClip(const CClipShape& clip);
	CClipShape getViewportClip() const;

	void endFrame();

	const shared_ptr<CDrawBuffer> getDrawBuffer() const;

private:

	uint32 makeListBatch();
	void writeRenderElementToBuffer(const CRenderElement& element);
	void writePolygonToBufferAsTris(const CVertex2D* verts, uint32 num);
	void writeClippedPolygonToBufferAsTris(const CVertex2D* verts, uint32 num, const CClipShape& clip);

	shared_ptr<CRenderList> _renderList;
	shared_ptr<CRenderBuffer> _renderBuffer;
	shared_ptr<CDrawBuffer> _drawBuffer;

	CClipShape _viewClip;
	CRenderState _currentRenderState;
	uint32 _numVertsRendered;
	bool _inFrame;
};

}