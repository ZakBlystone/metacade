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
rendertest.h: Temporary class for testing rendering
===============================================================================
*/

#pragma once

namespace Arcade
{

class CRenderTest : public IRenderTest, public CRuntimeObject
{
public:
	CRenderTest(CRuntimeObject* outer);
	virtual ~CRenderTest();

	bool init();

	virtual void frame(IRenderer *renderer, float time, CVec2 viewportsize);
	virtual void start(IRenderer *renderer);
	virtual void end(IRenderer *renderer);
	virtual void reloadVM();

	virtual void callFunction(CFunctionCall call);

private:
	ITexture* _testTexture;
	ITexture* _testTexture2;
	ITexture* _whiteTexture;

	IImage* _loadImage;
	IImage* _loadImage2;
	IImage* _whiteImage;

	shared_ptr<CElementRenderer> _renderer;
	IVMHost* _vmHost;

	IVMClass* _vmKlass;
	shared_ptr<IVMInstance> _vmInstance;

	float _lastTime;
};

}