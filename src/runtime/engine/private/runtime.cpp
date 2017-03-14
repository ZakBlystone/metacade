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
#include "script/lua/lua_private.h"

#include <fstream>

static shared_ptr<CElementRenderer> erender = nullptr;
static ITexture* testTexture = nullptr;
static ITexture* testTexture2 = nullptr;
static ITexture* whiteTexture = nullptr;
static CPackageManager* packageManager = nullptr;
static IRuntimeEnvironment* environment = nullptr;

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

	virtual uint32 getUniqueID() const override
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

	virtual uint32 getUniqueID() const override
	{
		return _id;
	}

private:
	uint8 *_bytes;
	int32 _width;
	int32 _height;
	uint32 _id;
};

static TestImage *loadImage, *loadImage2;
static WhiteImage *whiteImage;
static shared_ptr<IVMHost> vm;
static IVMClass *klass;
static IVMInstance *instance;

static void variantTest(const CVariant &v)
{
	std::cout << v.getTypeName() << std::endl;

	if ( v.type() == VT_STRING )
	{
		int32 len;
		const char *buff;
		if ( v.get(buff, len) )
		{
			std::cout << buff << std::endl;
		}
	}
	else if ( v.type() == VT_DOUBLE )
	{
		double value;
		if ( v.get(value) )
		{
			std::cout << value << std::endl;
		}
	}
}

bool CRuntime::initialize(IRuntimeEnvironment* env)
{
	environment = env;
	if ( environment == nullptr ) return false;

	packageManager = new CPackageManager(environment->getFileSystem());

	variantTest("Hello Variant");	
	variantTest(5.0);
	variantTest(10.2);

	erender = make_shared<CElementRenderer>();
	whiteImage = new WhiteImage;
	loadImage = new TestImage("E:/Temp/pic3.dat", 1);
	loadImage2 = new TestImage("E:/Temp/pic2.dat", 2);

	vm = getLuaVM();
	if ( vm->init() )
	{
		std::cout << "Initialized VM" << std::endl;
		klass = vm->loadGameVMClass();
		instance = klass->createVMInstance();
	}
	else
	{
		std::cout << "Failed to initialize VM" << std::endl;
	}

	return true;
}

void CRuntime::shutdown()
{
	delete whiteImage;
	delete loadImage;
	delete loadImage2;
	erender.reset();

	delete packageManager;
}

static float lastTime = 0.f;
void CRuntime::testRendering(IRenderer *renderer, float time, CVec2 viewportsize)
{
	float DT = time - lastTime;
	if ( DT > 1.f ) DT = 1.f;
	lastTime = time;

	instance->think(time, DT);

	erender->beginFrame();

	CClipShape viewClip;
	viewClip.add(CHalfPlane(CVec2(-1,0), CVec2(0,0)));
	viewClip.add(CHalfPlane(CVec2(0,-1), CVec2(0,0)));
	viewClip.add(CHalfPlane(CVec2(1,0), CVec2(viewportsize.x,0)));
	viewClip.add(CHalfPlane(CVec2(0,1), CVec2(0,viewportsize.y)));

	erender->setViewportClip(viewClip);

	instance->render(erender);

	erender->endFrame();

	renderer->render(erender->getDrawBuffer().get());
}

void Arcade::CRuntime::testRenderStart(IRenderer *renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	testTexture = provider->loadTexture(renderer, loadImage);
	testTexture2 = provider->loadTexture(renderer, loadImage2);
	whiteTexture = provider->loadTexture(renderer, whiteImage);
}

void Arcade::CRuntime::testRenderEnd(IRenderer *renderer)
{
	ITextureProvider* provider = renderer->getTextureProvider();
	if ( provider == nullptr ) return;

	provider->freeTexture(renderer, testTexture);
	provider->freeTexture(renderer, testTexture2);
	provider->freeTexture(renderer, whiteTexture);
}

void Arcade::CRuntime::reloadVM()
{
	if ( klass != nullptr && instance != nullptr )
	{
		klass->shutdownVMInstance(instance);
		klass->reload();
		instance = klass->createVMInstance();
	}
}

IPackageManager* CRuntime::getPackageManager()
{
	return packageManager;
}
