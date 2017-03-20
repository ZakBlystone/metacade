//#include "metacade_public.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

using std::ostream;
using std::istream;
using std::shared_ptr;
using std::make_shared;
using std::cout;
using std::endl;

#include "metacade_amal.h"

using namespace Arcade;

#include "SDL.h"
#include "GL/glew.h"
#include "GL/glew.c"
#include "IL/il.h"

#include "glrender.h"
#include "native.h"
#include <fstream>

int onError(const char *error)
{
	//When an error occurs, throw a message box up on the screen.
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error, NULL);
	SDL_Quit();
	return 1;
}

void checkError(int line)
{
	//Check errors from SDL
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		char buf[4096];
		sprintf(buf, "SDL_Error: %s + line: %i", error, line);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", buf, NULL);
		SDL_ClearError();
	}
}

static SDL_Window *window = NULL;
static SDL_GLContext glContext;
static shared_ptr<CRendererGL> renderer;

int initOpenGLAndWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_EVENTS);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);  //24-bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); //4x multi-sampling for pretty lines

	window = SDL_CreateWindow("Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) return onError("Failed To Init SDL_Window");


	checkError(__LINE__);
	glContext = SDL_GL_CreateContext(window);
	checkError(__LINE__);
	SDL_GL_SetSwapInterval(1);

	glewInit();
	return 0;
}

void buildImage(CImageAsset* asset, const CString& file)
{
	ILuint test = ilGenImage();
	ilBindImage(test);
	ilLoadImage(*file);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int width = (int) ilGetInteger(IL_IMAGE_WIDTH);
	int height = (int) ilGetInteger(IL_IMAGE_HEIGHT);
	
	uint8* data = (uint8*) ilGetData();
	asset->setImagePixels(PFM_RGBA8, 4, width, height, data);
}

bool buildImage2(CImageAsset* asset, IMetaData* params)
{
	ILuint test = ilGenImage();
	ilBindImage(test);
	if ( !ilLoadImage(*params->getValue("file")) ) return false;

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int width = (int) ilGetInteger(IL_IMAGE_WIDTH);
	int height = (int) ilGetInteger(IL_IMAGE_HEIGHT);
	
	uint8* data = (uint8*) ilGetData();
	asset->setImagePixels(PFM_RGBA8, 4, width, height, data);
	return true;
}

class CCompiler : public IAssetCompiler
{
	virtual bool compile(IAsset* asset, IMetaData* buildParameters) override
	{
		switch(asset->getType())
		{
		case ASSET_NONE:
		break;
		case ASSET_CODE:
		break;
		case ASSET_TEXTURE:
			return buildImage2((CImageAsset*) asset, buildParameters);
		break;
		case ASSET_SOUND:
		break;
		default:
		break;
		}

		return false;
	}
};
void testPackageStuff(IRuntime *system)
{
	IPackageManager* packmanager = system->getPackageManager();
	if(true)
	{
		CPackageBuilder* builder = packmanager->createPackageBuilder("MyPackage");
		builder->load();

		builder->getMetaData()->setKeyValuePair("name", "Another Package Test");
		builder->getMetaData()->setKeyValuePair("game", "My Test");
		builder->getMetaData()->setKeyValuePair("author", "Zak");

		{
			CCodeAsset* code = builder->addNamedAsset<CCodeAsset>("main.lua");
			code->setCodeBuffer("This is some code contained within a package, Yeah");
		}

		{
			CCodeAsset* code = builder->addNamedAsset<CCodeAsset>("aux.lua");
			code->setCodeBuffer("Some other code that isn't main");
		}

		{
			CCodeAsset* code = builder->addNamedAsset<CCodeAsset>("aux2.lua");
			code->setCodeBuffer("Whatever, here's more");
		}

		{
			CImageAsset* image = builder->addNamedAsset<CImageAsset>("test");
			buildImage(image, "E:/Temp/starblur.png");
		}

		{
			CImageAsset* image = builder->addNamedAsset<CImageAsset>("test2");
			buildImage(image, "E:/Temp/chair_icon0.png");
		}

		{
			//CImageAsset* image = builder->addNamedAsset<CImageAsset>("test2");
		}

		if ( builder->save() )
		{
			std::cout << "Saved OK" << std::endl;
		}
		else
		{
			std::cout << "Failed to save!" << std::endl;
		}

		packmanager->deletePackageBuilder(builder);
	}


	//if ( true ) return 0;


	if ( !packmanager->findAndPreloadPackages() )
	{
		std::cout << "Failed to load packages" << std::endl;
	}

	for ( uint32 i=0; i<packmanager->getNumPackages(); ++i )
	{
		IPackage* pkg = packmanager->getPackage(i);
		std::cout << "Num Assets: " << pkg->getNumAssets() << std::endl;

		for ( uint32 j=0; j < pkg->getMetaData()->getNumKeys(); ++j )
		{
			std::cout << "\t" << *pkg->getMetaData()->getKey(j) << ": " << *pkg->getMetaData()->getValue(j) << std::endl;
		}

		if ( pkg->loadAssets() )
		{
			std::cout << "Loaded Assets" << std::endl;
		}

		for ( uint32 j=0; j < pkg->getNumAssets(); ++j )
		{
			IAsset* asset = pkg->getAsset(j);

			std::cout << asset->getUniqueID().tostring() << "(" << *asset->getName() << "): ";

			if ( asset->getType() == ASSET_CODE )
			{
				CCodeAsset* code = (CCodeAsset* ) asset;
				std::cout << "[" << code->getCodeLength() << "]: " << (code->getCodeLength() ? code->getCodeBuffer() : "<no code>") << std::endl;
			}
			else if ( asset->getType() == ASSET_TEXTURE )
			{
				CImageAsset* image = (CImageAsset* ) asset;
				std::cout << "[" << image->getID() << "]: " << image->getWidth() << "x" << image->getHeight() << std::endl;
			}
		}
	}

	for ( uint32 i=0; i<packmanager->getNumPackages(); ++i )
	{
		IPackage* pkg = packmanager->getPackage(i);
		pkg->releaseAssets();
	}
}

int start(int argc, char *argv[])
{
	ilInit();

	shared_ptr<NativeEnv> native = make_shared<NativeEnv>();
	shared_ptr<CCompiler> assetCompiler = make_shared<CCompiler>();
	IRuntime *system;

	if ( Arcade::create(&system) && system->initialize(native.get()) )
	{
		cout << "Initialized" << endl;
	}
	else
	{
		return onError("Failed to init arcade runtime");;
	}

	std::cout << "Loading Packages..." << std::endl;
	IPackageManager* packmanager = system->getPackageManager();
	packmanager->setRootDirectory("E:/Projects/metacade/bin/Release");

	CPackageBuilder* builder = packmanager->createPackageBuilder("Default");
	builder->load();

	builder->setAssetCompiler(assetCompiler.get());
	builder->getMetaData()->setKeyValuePair("name", "Another Package Test");
	builder->getMetaData()->setKeyValuePair("game", "My Test");
	builder->getMetaData()->setKeyValuePair("author", "Zak");

	if ( !builder->setAndBuildMainScript("default.lua") )
	{
		std::cout << "Failed to build" << std::endl;
	}
	else
	{
		std::cout << "Build OK" << std::endl;
	}

	if ( builder->save() )
	{
		std::cout << "Saved OK" << std::endl;
	}
	else
	{
		std::cout << "Failed to save!" << std::endl;
	}

	packmanager->deletePackageBuilder(builder);

	//if ( true ) return 0;

	/*CPackage* package = packmanager->createPackage();

	{
		CCodeAsset* code = package->addAsset<CCodeAsset>();
		code->setCodeBuffer("Some code");
		std::cout << code->getUniqueID().tostring() << std::endl;
	}
	{
		CCodeAsset* code = package->addAsset<CCodeAsset>();
		code->setCodeBuffer("Some more code");
		std::cout << code->getUniqueID().tostring() << std::endl;
	}

	
	{
		IFileObject* packTest = native->getFileSystem()->openFile("pack2.mpkg", FILE_WRITE);
		if ( packTest != nullptr )
		{
			package->save(packTest);
			native->getFileSystem()->closeFile(packTest);
		}
	}*/

	/*{
		std::cout << "Num Assets: " << package->getNumAssets() << std::endl;

		for ( uint32 i=0; i<package->getNumAssets(); ++i )
		{
			std::cout << package->getAsset(i)->getUniqueID().tostring() << std::endl;
			if ( package->getAsset(i)->getType() == ASSET_CODE )
			{
				CCodeAsset* code = (CCodeAsset* ) package->getAsset(i);
				std::cout << code->getCodeLength() << ": " << code->getCodeBuffer() << std::endl;
			}
		}
	}*/

	//packmanager->deletePackage(package);

	/*std::map<CGUID, uint32> mapped;

	while(1)
	{
		CGUID x = CGUID::generate();

		if ( mapped.find(x) != mapped.end() )
		{
			std::cout << "COLLISION: " << mapped.size() << std::endl;
			return 1;
		}
		
		if ( mapped.size() % 10000 == 0)
		{
			std::cout << mapped.size() << ": " << x.tostring() << std::endl;
		}

		mapped.insert(std::make_pair(x, (uint32)mapped.size()));
	}*/

	if ( initOpenGLAndWindow() ) return 1;

	renderer = make_shared<CRendererGL>();
	renderer->reshape(800, 600);

	system->getRenderTest()->start(renderer.get());

	bool paused = false;
	bool running = true;
	float lastTime = 0;
	float deltaSeconds = 0;
	while (running)
	{
		SDL_PumpEvents();
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			if ( evt.type == SDL_QUIT ) 
			{
				running = false;
				break;
			}

			if ( evt.type == SDL_WINDOWEVENT )
			{
				if ( evt.window.event == SDL_WINDOWEVENT_RESIZED )
					renderer->reshape(evt.window.data1, evt.window.data2);
			}

			if ( evt.type == SDL_KEYDOWN )
			{
				bool pressed = evt.key.state == SDL_PRESSED;
				if ( pressed && evt.key.keysym.scancode == SDL_SCANCODE_R )
				{
					system->getRenderTest()->reloadVM();
				}
			}

			if ( evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP )
			{
				if ( evt.key.keysym.scancode == SDL_SCANCODE_E )
				{
					bool wasPaused = paused;
					paused = evt.key.state == SDL_PRESSED;
					bool changed = wasPaused != paused;

					if ( changed )
					{
					if ( paused )
					{
						system->getRenderTest()->end(renderer.get());
					}
					else
					{
						system->getRenderTest()->start(renderer.get());
					}
					}
				}
			}
		}

		float time = (float)(SDL_GetTicks()) / 1000.f;
		if (lastTime == 0) lastTime = time;
		deltaSeconds = time - lastTime;
		lastTime = time;

		//glClearColor(0.1f, 0.1f, 0.2f, 1.0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if ( !paused )
		{
			system->getRenderTest()->frame(renderer.get(), time, CVec2(400, 300));
			system->getRenderTest()->frame(renderer.get(), time, CVec2(400, 300));
		}

		SDL_GL_SwapWindow(window);

		Sleep(5);
	}

	system->getRenderTest()->end(renderer.get());

	ilShutDown();

	Arcade::destroy(system);

	return 0;
}

int SDL_main(int argc, char *argv[])
{
	return start(argc, argv);
}
