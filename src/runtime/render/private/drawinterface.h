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
drawinterface.h:
===============================================================================
*/

#pragma once

#include "render/private/renderbuffer.h"
#include "render/public/renderelement.h"
#include "engine/public/assets/imageresource.h"

namespace Arcade
{

class CDrawInterface
{
public:
	void init();
	void shutdown();

	void setRenderer(shared_ptr<CElementRenderer> renderer);

	void reset();

	// transform
	bool pushTransform();
	bool popTransform();

	void translate(float x, float y);
	void rotate(float r);
	void scale(float sx, float sy);

	// clipping
	bool pushClipRect(
		float x,
		float y,
		float w,
		float h);

	bool pushClipPlane(
		float x,
		float y,
		float nx,
		float ny);

	bool popClip();

	// state
	void setColor(const CColor& color);
	void setBlendMode(EBlendMode blend);
	void setLayer(int32 layer);

	CColor getColor() const;

	// drawing
	void drawRect(
		float x, 
		float y, 
		float w, 
		float h, 
		CImageAsset* texture = nullptr, 
		float u0 = 0.f, 
		float v0 = 0.f, 
		float u1 = 1.f, 
		float v1 = 1.f);

	void drawSprite(
		float x, 
		float y, 
		float w, 
		float h, 
		float r,
		CImageAsset* texture = nullptr, 
		float u0 = 0.f, 
		float v0 = 0.f, 
		float u1 = 1.f, 
		float v1 = 1.f);

	void drawQuad(
		const CRenderQuad& quad, 
		CImageAsset* texture = nullptr);

	// misc
	CVec2 getSize();

private:
	shared_ptr<CElementRenderer> _renderer;

	struct CTransformClipStack;
	CTransformClipStack* _stack;

	int32 _layer;
	CRenderState _renderState;
	CColor _color;
};

}