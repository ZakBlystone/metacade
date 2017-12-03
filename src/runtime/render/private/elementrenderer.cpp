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

CElementRenderer::CElementRenderer()
{
	_renderList = makeShared<CRenderList>();
	_renderBuffer = makeShared<CRenderBuffer>();
	_drawBuffer = makeShared<CDrawBuffer>(_renderBuffer);
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
		_renderBuffer->addVertex(verts[i] * _viewportTransform);
	}

	for ( uint32 i=1; i<num-1; ++i )
	{
		_renderBuffer->addIndex(baseIndex);
		_renderBuffer->addIndex(baseIndex + (uint16) i);
		_renderBuffer->addIndex(baseIndex + (uint16) i + 1);
	}
}

void CElementRenderer::writeClippedPolygonToBufferAsTris(const CVertex2D* verts, uint32 num, const CClipShape& clip)
{
	//Minimum of 3 vertices required
	if ( num < 3 ) return;

	//Check if clipping even needs to be done
	if ( clip.getNumPlanes() == 0 )
	{
		//writePolygonToBufferAsTris(verts, num);
		return;
	}

	//#TODO Optimize memory allocation here, use CRuntimeObject's allocator
	vector<CVertex2D> outputList, inputList;
	outputList.reserve(num);
	outputList.resize(num);
	memcpy(outputList.data(), verts, num * sizeof(CVertex2D));

	//Iterate over each clipping plane
	for ( int32 i=0; i<clip.getNumPlanes(); ++i )
	{
		const CHalfPlane& plane = clip.getHalfPlane(i);
		
		//Move output list into an inputList
		inputList = std::move(outputList);

		//Exit early if there is nothing left to clip
		if ( inputList.size() == 0 ) break;

		//Start from the last vertex in the list
		CVertex2D v0 = inputList.back();

		//Classify first vertex against clipping plane
		EPointClassify v0P = plane.classifyPoint(v0._position);

		//Iterate over polygon
		for ( const CVertex2D& v1 : inputList )
		{
			//Classify subsequent vertices against clipping plane
			EPointClassify v1P = plane.classifyPoint(v1._position);

			//Case where a vertex is in behind (inside) the plane
			if ( v1P == PLANE_BEHIND )
			{
				//Case where edge (v0 -> v1) is entering clipping plane from the outside
				if ( v0P == PLANE_INFRONT )
				{
					float frac;
					plane.intersection(v0._position, v1._position, frac);

					CVertex2D newVertex;
					CVertex2D::interpolateTo(v0, v1, newVertex, frac );

					//Add interpolated vertex (v0 -> | X | -> v1)
					outputList.push_back( newVertex );
				}

				//Add the vertex which is inside the clipping plane.
				//if the previous case doesn't execute, the whole edge is inside.
				outputList.push_back(v1);
			}
			//Case where edge (v0 -> v1) is leaving clipping plane
			else if ( v0P == PLANE_BEHIND )
			{
				float frac;
				plane.intersection(v1._position, v0._position, frac);

				CVertex2D newVertex;
				CVertex2D::interpolateTo(v1, v0, newVertex, frac );

				//Add interpolated vertex (v1 -> | X | -> v0)
				outputList.push_back( newVertex );
			}

			//v1 is now v0 for the next iteration
			v0 = v1;
			v0P = v1P;
		}
	}

	if ( outputList.empty() ) return;

	writePolygonToBufferAsTris(outputList.data(), (uint32) outputList.size());
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

void CElementRenderer::setViewportTransform(const CMatrix3& matrix)
{
	_viewportTransform = matrix;
}

CMatrix3 CElementRenderer::getViewportTransform() const
{
	return _viewportTransform;
}
