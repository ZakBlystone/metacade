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
drawinterface.cpp:
===============================================================================
*/

#include "render/render_private.h"

#define DI_RENDER_CHECK\
	if ( _renderer == nullptr ) return;

#define NUM_MATRIX_STACK 16
#define NUM_CLIP_STACK 32

struct CDrawInterface::CTransformClipStack
{
	CMatrix3 _xformStack[NUM_MATRIX_STACK];
	CClipShape _clipStack[NUM_CLIP_STACK];
	uint32 _xformStackPos;
	uint32 _clipStackPos;

	void resetStacks()
	{
		_xformStackPos = NUM_MATRIX_STACK - 1;
		_xformStack[_xformStackPos] = CMatrix3();

		_clipStackPos = NUM_CLIP_STACK - 1;
		_clipStack[_clipStackPos] = CClipShape();
	}

	bool xformPush()
	{
		if ( _xformStackPos == 0 ) return false;

		CMatrix3& copy = _xformStack[_xformStackPos];
		_xformStack[--_xformStackPos] = copy;

		return true;
	}

	bool xformPop()
	{
		if ( _xformStackPos == NUM_MATRIX_STACK - 1 ) return false;
		++_xformStackPos;

		return true;
	}

	bool clipPush()
	{
		if ( _clipStackPos == 0 ) return false;

		CClipShape& copy = _clipStack[_clipStackPos];
		_clipStack[--_clipStackPos] = copy;

		return true;
	}

	bool clipPop()
	{
		if (_clipStackPos == NUM_MATRIX_STACK - 1) return false;
		++_clipStackPos;

		return true;
	}

	CMatrix3& xformTop()
	{
		return _xformStack[_xformStackPos];
	}

	CClipShape& clipTop()
	{
		return _clipStack[_clipStackPos];
	}
};

void CDrawInterface::init()
{
	_stack = construct<CTransformClipStack>();
	reset();
}

void CDrawInterface::shutdown()
{
	destroy(_stack);
}

void CDrawInterface::setRenderer(shared_ptr<CElementRenderer> renderer)
{
	_renderer = renderer;
}

void CDrawInterface::reset()
{
	_stack->resetStacks();
	_layer = 0;
	_renderState._material = CMaterial();
	_color = CColor(0xFFFFFFFF);

	if ( _renderer != nullptr )
		_stack->clipTop() = _renderer->getViewportClip();
}

bool CDrawInterface::pushTransform()
{
	return _stack->xformPush();
}

bool CDrawInterface::popTransform()
{
	return _stack->xformPop();
}

void CDrawInterface::translate(float x, float y)
{
	_stack->xformTop().translate( CVec2(x,y) );
}

void CDrawInterface::rotate(float r)
{
	_stack->xformTop().rotate( r );
}

void CDrawInterface::scale(float sx, float sy)
{
	_stack->xformTop().scale( CVec2(sx, sy) );
}

bool CDrawInterface::pushClipRect(float x, float y, float w, float h)
{
	if (!_stack->clipPush())
		return false; //"Draw clip stack overflow";

	CClipShape newshape;
	newshape.add(CHalfPlane(CVec2(-1, 0), CVec2(x, 0)));
	newshape.add(CHalfPlane(CVec2(0, -1), CVec2(0, y)));
	newshape.add(CHalfPlane(CVec2(1, 0), CVec2(x+w, 0)));
	newshape.add(CHalfPlane(CVec2(0, 1), CVec2(0, y+h)));

	_stack->clipTop() -= newshape;
	return true;
}

bool CDrawInterface::pushClipPlane(float x, float y, float nx, float ny)
{
	if (!_stack->clipPush())
		return false; //"Draw clip stack overflow";

	CClipShape newshape;
	newshape.add(CHalfPlane(CVec2(nx, ny), CVec2(x, y)));

	_stack->clipTop() -= newshape;
	return true;
}

bool CDrawInterface::popClip()
{
	return _stack->clipPop();
}

void CDrawInterface::setColor(const CColor& color)
{
	_color = color;
}

void CDrawInterface::setBlendMode(EBlendMode blend)
{
	_renderState._material._blend = blend;
}

void CDrawInterface::setLayer(int32 layer)
{
	_layer = layer;
}

CColor CDrawInterface::getColor() const
{
	return _color;
}

void CDrawInterface::drawRect(float x, float y, float w, float h, CImageAsset* texture /*= nullptr*/, float u0 /*= 0.f*/, float v0 /*= 0.f*/, float u1 /*= 1.f*/, float v1 /*= 1.f*/)
{
	DI_RENDER_CHECK

	/*float hw = w * .5f;
	float hh = h * .5f;
	float cx = x + hw;
	float cy = y + hh;*/

	/*int32 ccheck = 0;
	for (int32 i=0; i<gData._viewClip.getNumPlanes(); ++i )
	{
		const CHalfPlane& p = gData._viewClip.getHalfPlane(i);
		const float lcx = cx * p.x;
		const float lcy = cy * p.y;
		const float lw = hw * fabs(p.x);
		const float lh = hh * fabs(p.y);
		if ( lcx - lw + lcy - lh - p.z > 0 )
		{
			return 0;
		}
		else if ( lcx + lw + lcy + lh - p.z > 0 )
		{
			++ccheck;
		}
	}*/

	/*if ( ccheck == 0 )
	{
		static CClipShape empty;
		quad = &el.makeQuad2(empty, gData, gData._layer);
		verts = quad->_verts;
	}
	else
	{*/	

	_renderState._material._baseTexture = texture != nullptr ? texture->getID() : 0;

	CRenderElement& el = _renderer->addRenderElement();
	CRenderQuad* quad = &el.makeQuad2(_stack->clipTop(), _renderState, _layer);
	CVertex2D* verts = quad->_verts;

	verts[0]._position._x = x;
	verts[0]._position._y = y;
	verts[0]._texcoord._x = u0;
	verts[0]._texcoord._y = v0;
	verts[0]._color._irgba = _color._irgba;

	verts[1]._position._x = x+w;
	verts[1]._position._y = y;
	verts[1]._texcoord._x = u1;
	verts[1]._texcoord._y = v0;
	verts[1]._color._irgba = _color._irgba;

	verts[2]._position._x = x+w;
	verts[2]._position._y = y+h;
	verts[2]._texcoord._x = u1;
	verts[2]._texcoord._y = v1;
	verts[2]._color._irgba = _color._irgba;

	verts[3]._position._x = x;
	verts[3]._position._y = y+h;
	verts[3]._texcoord._x = u0;
	verts[3]._texcoord._y = v1;
	verts[3]._color._irgba = _color._irgba;

	quad->transform(_stack->xformTop());
}

void CDrawInterface::drawSprite(float x, float y, float w, float h, float r, CImageAsset* texture /*= nullptr*/, float u0 /*= 0.f*/, float v0 /*= 0.f*/, float u1 /*= 1.f*/, float v1 /*= 1.f*/)
{
	DI_RENDER_CHECK

	_renderState._material._baseTexture = texture != nullptr ? texture->getID() : 0;

	CMatrix3 xform;
	CMatrix3::identity(xform);

	xform.rotate(r);
	xform.translate(CVec2(x,y));

	CRenderQuad quad;
	quad.makeBox(CVec2(-w,-h), CVec2(w,h), _color);
	quad.transform(_stack->xformTop() * xform);

	quad._verts[0]._texcoord.set(u0, v0);
	quad._verts[1]._texcoord.set(u1, v0);
	quad._verts[2]._texcoord.set(u1, v1);
	quad._verts[3]._texcoord.set(u0, v1);

	CRenderElement& el = _renderer->addRenderElement();
	el.makeQuad(quad, _stack->clipTop(), _renderState, _layer);
}

void CDrawInterface::drawQuad(const CRenderQuad& quad, CImageAsset* texture /*= nullptr*/)
{
	DI_RENDER_CHECK

	_renderState._material._baseTexture = texture != nullptr ? texture->getID() : 0;

	CRenderQuad transformedQuad = quad;
	transformedQuad.transform( _stack->xformTop() );

	CRenderElement& el = _renderer->addRenderElement();
	el.makeQuad(transformedQuad, _stack->clipTop(), _renderState, _layer);
}

CVec2 CDrawInterface::getSize()
{
	if ( _renderer == nullptr ) return Vec2Zero;
	return _renderer->getViewSize();
}
