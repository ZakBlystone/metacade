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
glrender.h:
===============================================================================
*/

#pragma once

//#include "render/render_public.h"
#include "metacade_amal.h"

using namespace Arcade;

class CRendererGL : public IRenderer
{
public:
	CRendererGL();
	~CRendererGL();

	void reshape(int32 width, int32 height);

	virtual void render(class IDrawBuffer* buffer) override;
	virtual ITexture* loadTexture(class IImage* imagesource) override;
	virtual void freeTexture(ITexture* texture) override;

private:
	void renderBatch(IDrawBuffer* buffer, const CRenderBatch* batch);
	void updateRenderState(uint32 stateChangeFlags, const CRenderState& newState);
};