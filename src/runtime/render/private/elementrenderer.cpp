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
elementrenderer.cpp:
===============================================================================
*/

#include "render_private.h"

CElementRenderer::CElementRenderer(CRuntimeObject* outer)
	: CRuntimeObject(outer)
{
	_renderList = make_shared<CRenderList>();
	_renderBuffer = make_shared<CRenderBuffer>();
	_drawBuffer = make_shared<CDrawBuffer>(_renderBuffer);
}

void CElementRenderer::beginFrame()
{
	if ( _inFrame ) return;

	_renderList->clear();
	_renderBuffer->clear();
	_drawBuffer->clearRenderBatches();
	_currentRenderState = CRenderState(); //make this have a good default value
	_inFrame = true;
}

CRenderElement& CElementRenderer::addRenderElement()
{
	return _renderList->emplaceRenderElement();
}

void CElementRenderer::endFrame()
{
	if ( !_inFrame ) return;

	_renderList->sort();
	while ( !_renderList->empty() && makeListBatch() != 0 )
	{
	}

	_numVertsRendered = _renderBuffer->getNumVertices();
	_inFrame = false;
}

const shared_ptr<CDrawBuffer> CElementRenderer::getDrawBuffer() const
{
	return _drawBuffer;
}

uint32 CElementRenderer::makeListBatch()
{
	uint16 baseVertex = _renderBuffer->getNumVertices();
	uint32 baseIndex = _renderBuffer->getNumIndices();
	CRenderState baseState = _renderList->top().getRenderState();

	uint32 inserted = 0;
	while ( !_renderList->empty() )
	{
		CRenderElement& element = _renderList->top();
		uint32 stateChange = element.getRenderState().getStateChangeFlags(baseState);

		if ( stateChange != 0 ) break;

		++inserted;

		writeRenderElementToBuffer(element);
		_renderList->pop();
	}

	if ( _renderBuffer->getNumIndices() == baseIndex )
	{
		//don't submit a render batch, nothing was indexed
		return inserted;
	}

	//#TODO Primitives need to come from render elements somehow
	CRenderBatch newBatch(baseIndex, _renderBuffer->getNumIndices() - baseIndex, baseState, PRIM_TRIANGLES);
	_drawBuffer->addRenderBatch(newBatch);

	return inserted;
}


void CElementRenderer::writeRenderElementToBuffer(const CRenderElement& element)
{
	switch ( element.getType() )
	{
		case RET_NONE:
			//nothing to do for uninitialized elements
		break;
		case RET_QUAD:
			writeClippedPolygonToBufferAsTris(element.getQuad()._verts, 4, element.getClipShape());
		break;
	}
}

void CElementRenderer::writePolygonToBufferAsTris(const CVertex2D* verts, uint32 num)
{
	uint16 baseIndex = _renderBuffer->getNumVertices();

	if ( num <= 2) return; //not enough points
	for ( uint32 i=0; i<num; ++i )
	{
		_renderBuffer->addVertex(verts[i]);
	}

	for ( uint32 i=1; i<num-1; ++i )
	{
		_renderBuffer->addIndex(baseIndex);
		_renderBuffer->addIndex(baseIndex + i);
		_renderBuffer->addIndex(baseIndex + i + 1);
	}
}

void CElementRenderer::writeClippedPolygonToBufferAsTris(const CVertex2D* verts, uint32 num, const CClipShape& clip)
{
	if ( clip.getNumPlanes() == 0 )
	{
		writePolygonToBufferAsTris(verts, num);
		return;
	}

	vector<CVertex2D> vlist(num);
	memcpy(vlist.data(), verts, num * sizeof(CVertex2D));

	bool clipped = false;
	bool visible = true;
	for ( int32 i=0; i<clip.getNumPlanes(); ++i )
	{
		if ( visible )
		{
			EPointClassify pc = CVertexClipper::clipPolygonAgainstPlane(vlist, clip.getHalfPlane(i));
			if ( pc == PLANE_INTERSECT ) clipped = true;
			if ( pc == PLANE_INFRONT ) visible = false;
		}
	}

	if ( !visible ) return;

	writePolygonToBufferAsTris(vlist.data(), vlist.size());
}

void Arcade::CElementRenderer::setViewportClip(const CClipShape& clip)
{
	_viewClip = clip;
}

Arcade::CClipShape Arcade::CElementRenderer::getViewportClip() const
{
	return _viewClip;
}

