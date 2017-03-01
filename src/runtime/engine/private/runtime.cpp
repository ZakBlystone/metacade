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

void CRuntime::testRendering(IRenderer *renderer, float time)
{
	erender->beginFrame();

	CRenderQuad testQuad;
	CClipShape testClip;
	CRenderState state;

	testClip.add(CHalfPlane(CVec2(1.f, 1.f).normalize(), CVec2(600,0)));
	testClip.add(CHalfPlane(CVec2(-1.f, -1.f).normalize(), CVec2(200,0)));

	testQuad.makeBox(CVec2(0,0), CVec2(800,600), CColor(0x220033FF));
	erender->addRenderElement().makeQuad(testQuad, testClip, state, -1);

	for ( int32 y=0; y<20; ++y)
	for ( int32 i=0; i<100; ++i )
	{
		CVec2 pos((float) (i)*10.f, 30.f + sinf(time * 3.f + (float) (i + y * 6.f) / 10.f) * 30 + (y * 50.f));

		if ( i % 10 > 5 )
			state._material._blend = BLEND_ADD;
		else
			state._material._blend = BLEND_NORMAL;

		testQuad.makeBox(pos + CVec2(0,0), pos + CVec2(12,30), CColor(0x22CC2222));
		erender->addRenderElement().makeQuad(testQuad, testClip, state);
	}

	erender->endFrame();

	renderer->render(erender->getDrawBuffer().get());
}
