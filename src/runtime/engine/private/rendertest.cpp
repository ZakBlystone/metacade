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
rendertest.cpp: Temporary class for testing rendering
===============================================================================
*/

#include "engine_private.h"

#include "render/render_private.h"
#include "script/lua/lua_private.h"

#include <fstream>

class WhiteImage : public IImage
{
	virtual int32 getWidth() const override { return 2; }
	virtual int32 getHeight() const override { return 2; }
	virtual int32 getBytesPerPixel() const override { return 4; }
	virtual EImagePixelFormat getPixelFormat() const override
	{
		return EImagePixelFormat::PFM_RGBA8;
	}

	virtual uint8* getPixels() const override
	{
		static const CColor pixels[4] = {
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
			CColor(0xFFFFFFFF),
		};
		return (uint8 *) pixels;
	}

	virtual uint32 getID() const override
	{
		return 0;
	}

	virtual uint32 getFlags() const override
	{
		return 0;
	}
};

class TestImage : public IImage
{
public:

	TestImage(const char *path, uint32 id)
	{
		_id = id;

		std::fstream input(path, std::ios::in | std::ios::binary);
		input.read((char*) &_width, 4);
		input.read((char*) &_height, 4);

		_bytes = new uint8[_width * _height * 4];
		input.read((char *)_bytes, _width * _height * 4);
		input.close();
	}
	
	virtual ~TestImage()
	{
		delete [] _bytes;
	}

	virtual int32 getWidth() const override { return _width; }
	virtual int32 getHeight() const override { return _height; }
	virtual int32 getBytesPerPixel() const override { return 4; }
	virtual EImagePixelFormat getPixelFormat() const override
	{
		return EImagePixelFormat::PFM_RGBA8;
	}

	virtual uint8* getPixels() const override
	{
		return (uint8 *) _bytes;
	}

	virtual uint32 getID() const override
	{
		return _id;
	}

	virtual uint32 getFlags() const override
	{
		return 0;
	}

private:
	uint8 *_bytes;
	int32 _width;
	int32 _height;
	uint32 _id;
};

CRenderTest::CRenderTest(CRuntimeObject* outer)
	: CRuntimeObject(outer)
	, _vmInstance(nullptr)
{

	_whiteImage = new WhiteImage;
	_loadImage = new TestImage("E:/Temp/pic3.dat", 1);
	_loadImage2 = new TestImage("E:/Temp/pic2.dat", 2);

	_renderer = make_shared<CElementRenderer>(this);
	_lastTime = 0.f;
}

CRenderTest::~CRenderTest()
{
	delete _whiteImage;
	delete _loadImage;
	delete _loadImage2;
}

bool Arcade::CRenderTest::init()
{
	_vmHost = getLuaVM();
	//_vmKlass = _vmHost->loadGameVMClass();

	shared_ptr<IVMClass> klass = _vmKlass.lock();
	if ( klass != nullptr )
	{
		klass->reload();
		_vmInstance = klass->createVMInstance();
	}

	return true;
}

void CRenderTest::callFunction(CFunctionCall call)
{
	_vmInstance->callFunction(call);
}

void CRenderTest::reloadVM()
{
	shared_ptr<IVMClass> klass = _vmKlass.lock();
	if ( klass != nullptr )
	{
		klass->reload();
		_vmInstance = klass->createVMInstance();
	}
}

void CRenderTest::frame(IRenderer *renderer, float time, CVec2 viewportsize)
{
	float DT = time - _lastTime;
	if ( DT > 1.f ) DT = 1.f;
	_lastTime = time;

	_vmInstance->think(time, DT);

	_renderer->setViewSize(viewportsize);
	_renderer->beginFrame();

	CClipShape viewClip;
	viewClip.add(CHalfPlane(CVec2(-1,0), CVec2(0,0)));
	viewClip.add(CHalfPlane(CVec2(0,-1), CVec2(0,0)));
	viewClip.add(CHalfPlane(CVec2(1,0), CVec2(viewportsize.x,0)));
	viewClip.add(CHalfPlane(CVec2(0,1), CVec2(0,viewportsize.y)));

	_renderer->setViewportClip(viewClip);

	_vmInstance->render(_renderer);

	_renderer->endFrame();

	renderer->render(_renderer->getDrawBuffer().get());
}

void CRenderTest::start(IRenderer *renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	_testTexture = provider->loadTexture(renderer, _loadImage);
	_testTexture2 = provider->loadTexture(renderer, _loadImage2);
	_whiteTexture = provider->loadTexture(renderer, _whiteImage);
}

void CRenderTest::end(IRenderer *renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	provider->freeTexture(renderer, _testTexture);
	provider->freeTexture(renderer, _testTexture2);
	provider->freeTexture(renderer, _whiteTexture);
}
