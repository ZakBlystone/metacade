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
			//writePolygonToBufferAsTris(element.getQuad()._verts, 4);
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

struct VLLN
{
	VLLN() : next(nullptr)
	{}

	CVertex2D vert;
	EPointClassify pc;
	struct VLLN* next;
};

struct VLL
{
	VLLN freenodes[MAX_VERTICES];
	VLLN *root, *freelist;

	VLL()
		: freelist(freenodes)
		, root(nullptr)
	{
		for ( uint32 i=0; i<MAX_VERTICES-1; ++i )
			freenodes[i].next = &freenodes[i+1];
	}

	void reset()
	{
		while(root != nullptr)
		{
			VLLN* next = root->next;
			root->next = freelist;
			freelist = root;
			root = next;
		}
	}

	void free(VLLN* node)
	{
		if ( node == root ) root = root->next;

		node->next = freelist;
		freelist = node;
	}

	VLLN* insert(VLLN* after)
	{
		VLLN* newNode = alloc();
		if ( newNode == nullptr ) return nullptr;

		newNode->next = after->next;
		after->next = newNode;
		return newNode;
	}

	VLLN* forward(VLLN* node)
	{
		if ( node->next != nullptr ) return node->next;
		return root;
	}

	VLLN* alloc()
	{
		if ( freelist != nullptr )
		{
			VLLN* ret = freelist;
			freelist = freelist->next;

			return ret;
		}
		return nullptr;
	}

	VLLN* append()
	{
		VLLN* ret = alloc();
		if ( ret == nullptr ) return nullptr;

		ret->next = root;
		root = ret;	
		return ret;
	}

	VLLN* first() { return root; }
};

static EPointClassify clipPolygonAgainstPlane(const CHalfPlane& plane, VLL& list)
{
	float frac;
	uint32 i;
	uint32 num_behind = 0;
	uint32 num_infront = 0;

	VLLN* node = list.first();
	if ( node == nullptr ) return PLANE_INFRONT;

	i=0;
	while(node)
	{
		node->pc = plane.clasifyPoint(node->vert._position);
		node->pc == PLANE_BEHIND ? ++num_behind : ++num_infront;
		node = node->next;
		++i;
	}

	if ( num_behind == 0 ) return PLANE_INFRONT;
	if ( num_infront == 0 ) return PLANE_BEHIND;

	node = list.first();
	while(node)
	{
		VLLN* next = list.forward(node);

		const CVertex2D *v0 = &node->vert;
		const CVertex2D *v1 = &next->vert;

		EPointClassify &p0 = node->pc;
		EPointClassify &p1 = next->pc;

		if ( p0 == PLANE_INFRONT && p1 == PLANE_INFRONT ) 
		{
			VLLN* after = list.forward(next);
			if ( after != node && after->pc == PLANE_BEHIND )
			{
				node = node->next;
				continue;
			}

			node->next = nullptr;
			if ( after != list.root && next != list.root )
			{
				node->next = after;
			}

			VLLN* link = next->next;
			list.free(next);

			continue;
		}
		else if ( p0 == PLANE_INFRONT && p1 == PLANE_BEHIND )
		{
			plane.intersection(v0->_position, v1->_position, frac);

			if ( node == list.root ) node = list.insert(node);

			node->vert = v0->interpolateTo(*v1, frac);
		}
		else if ( p0 == PLANE_BEHIND && p1 == PLANE_INFRONT )
		{
			plane.intersection(v1->_position, v0->_position, frac);
			node = list.insert(node);
			node->vert = v1->interpolateTo(*v0, frac);
			node->pc = PLANE_INFRONT;
			continue;
		}

		node = node->next;
	}

	return PLANE_INTERSECT;
}

void CElementRenderer::writeClippedPolygonToBufferAsTris(const CVertex2D* verts, uint32 num, const CClipShape& clip)
{
	static VLL vertList;

	if ( clip.getNumPlanes() == 0 )
	{
		writePolygonToBufferAsTris(verts, num);
		return;
	}

	CRenderBuffer* buffer = _renderBuffer.get();

	vertList.reset();
	for ( int32 i=num-1; i>=0; --i )
	{
		vertList.append()->vert = verts[i];
	}

	bool clipped = false;
	bool visible = true;
	for ( int32 i=0; i<clip.getNumPlanes(); ++i )
	{
		if ( visible )
		{
			EPointClassify pc = clipPolygonAgainstPlane(clip.getHalfPlane(i), vertList);
			if ( pc == PLANE_INTERSECT ) clipped = true;
			if ( pc == PLANE_INFRONT ) visible = false;
		}
	}

	if ( !visible ) return;

	VLLN* node = vertList.first();
	if ( node == nullptr ) return;

	uint32 size = 0;
	uint32 mark = buffer->getNumVertices();
	while(node)
	{
		buffer->addVertex(node->vert);
		node = node->next;
		++size;
	}
	for ( uint32 i=1; i<size-1; ++i )
	{
		buffer->addIndex(mark);
		buffer->addIndex(mark + i);
		buffer->addIndex(mark + i + 1);
	}
}

void Arcade::CElementRenderer::setViewSize(const CVec2& size)
{
	_viewSize = size;
}

void Arcade::CElementRenderer::setViewportClip(const CClipShape& clip)
{
	_viewClip = clip;
}

Arcade::CVec2 Arcade::CElementRenderer::getViewSize() const
{
	return _viewSize;
}

Arcade::CClipShape Arcade::CElementRenderer::getViewportClip() const
{
	return _viewClip;
}

