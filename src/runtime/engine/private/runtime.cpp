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
runtime.cpp:
===============================================================================
*/

#include "engine_private.h"
#include "render/render_private.h"

static CElementRenderer* erender = nullptr;
static ITexture* testTexture = nullptr;

class TestImage : public IImage
{

public:
	virtual int32 getWidth() const override { return 4; }
	virtual int32 getHeight() const override { return 4; }
	virtual int32 getBytesPerPixel() const override { return 4; }
	virtual EImagePixelFormat getPixelFormat() const override
	{
		return EImagePixelFormat::PFM_RGBA8;
	}

	virtual uint8* getPixels() const override
	{
		static const CColor pixels[16] =
		{
			CColor(0xFF0000FF),
			CColor(0xFF0000FF),
			CColor(0xFF0000FF),
			CColor(0xFF0000FF),
			CColor(0x00FF00FF),
			CColor(0x00FF00FF),
			CColor(0x00FF00FF),
			CColor(0x00FF00FF),
			CColor(0x0000FFFF),
			CColor(0x0000FFFF),
			CColor(0x0000FFFF),
			CColor(0x0000FFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
		};

		return (uint8 *) pixels;
	}

	virtual uint32 getUniqueID() const override
	{
		return 0;
	}
};

static TestImage loadImage;

bool CRuntime::initialize()
{
	erender = new CElementRenderer;
	return true;
}

void CRuntime::shutdown()
{
	delete erender;
	erender = nullptr;
}

void CRuntime::testRendering(IRenderer *renderer, float time, CVec2 viewportsize)
{
	erender->beginFrame();

	CRenderQuad testQuad;
	CClipShape testClip;
	CRenderState state;
	CMatrix3 scratch;

	testClip.add(CHalfPlane(CVec2(-1,0), CVec2(0,0)));
	testClip.add(CHalfPlane(CVec2(0,-1), CVec2(0,0)));
	testClip.add(CHalfPlane(CVec2(1,0), CVec2(viewportsize.x,0)));
	testClip.add(CHalfPlane(CVec2(0,1), CVec2(0,viewportsize.y)));

	//testClip.add(CHalfPlane(CVec2(1.f, 1.f).normalize(), CVec2(600,0)));
	//testClip.add(CHalfPlane(CVec2(-1.f, -1.f).normalize(), CVec2(200,0)));

	state._material._blend = BLEND_NORMAL;
	if ( testTexture != nullptr )
	{
		state._material._baseTexture = testTexture->getID();
	}

	testQuad.makeBox(CVec2(0,0), CVec2(viewportsize.x,viewportsize.y), CColor(0x220033FF));
	erender->addRenderElement().makeQuad(testQuad, testClip, state, -1);

	for ( int32 y=0; y<20; ++y)
	for ( int32 i=0; i<100; i += 3 )
	{
		CVec2 pos((float) (i)*10.f, 30.f + sinf(time * 3.f + (float) (i + y * 6.f) / 10.f) * 30 + (y * 50.f));

		if ( i % 10 > 5 )
			state._material._blend = BLEND_ADD;
		else
			state._material._blend = BLEND_NORMAL;

		CMatrix3::identity(scratch);
		scratch.rotate(time + y + sin((float)(i)/5.f + time * .2f) * 40.f);
		scratch.translate(pos);

		testQuad.makeBox(CVec2(-15,-15), CVec2(25,15), CColor(0xFFFFFFFF)); //CColor(0x22CC22FF)
		testQuad.transform(scratch);

		erender->addRenderElement().makeQuad(testQuad, testClip, state);
	}

	erender->endFrame();

	renderer->render(erender->getDrawBuffer().get());
}

void Arcade::CRuntime::testRenderStart(IRenderer *renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	testTexture = provider->loadTexture(renderer, &loadImage);
}

void Arcade::CRuntime::testRenderEnd(IRenderer *renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	provider->freeTexture(renderer, testTexture);
}
