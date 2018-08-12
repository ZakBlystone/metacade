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
#include <map>
#include <algorithm>

using namespace Arcade;

class CRendererGL : public IRenderer, ITextureProvider
{
public:
	CRendererGL();
	~CRendererGL();

	void reshape(int32 width, int32 height);
	void clear();

	virtual void render(class IGameInstance* instance, class IDrawBuffer* buffer) override;
	virtual class ITextureProvider* getTextureProvider() override;

	virtual class ITexture* loadTexture(class IRenderer* renderContext, class IImage* imagesource) override;
	virtual void freeTexture(class IRenderer* renderContext, ITexture* texture) override;

	void setOffset(CVec2 offset);

	//void renderGUI(struct ImDrawData* drawData);

private:
	void renderBatch(const CRenderBatch* batch);
	void updateRenderState(uint32 stateChangeFlags, const CRenderState& newState);

	std::map<uint32, ITexture*> _textureRemap;
};