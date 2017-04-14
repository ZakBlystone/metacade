//#include "metacade_public.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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

#include "compiler.h"
#include "glrender.h"
#include "native.h"
#include "projectmanager.h"
#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include <fstream>

#define C_ARRAYSIZE(_ARR)      ((int)(sizeof(_ARR)/sizeof(*_ARR)))

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
static IPackage* loadedPackage = nullptr;
static IGameClass* loadedGameClass = nullptr;

static int initOpenGLAndWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_EVENTS);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);  //24-bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); //4x multi-sampling for pretty lines

	window = SDL_CreateWindow("Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) return onError("Failed To Init SDL_Window");


	checkError(__LINE__);
	glContext = SDL_GL_CreateContext(window);
	checkError(__LINE__);
	SDL_GL_SetSwapInterval(1);

	glewInit();

	ImGui_ImplSdlGL3_Init(window);

	return 0;
}

static void immediateUI(int32 width, int32 height)
{
	static bool bNewPackageDialog = false;

	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu("File") )
		{
			if ( ImGui::MenuItem("New Package") ) 
				bNewPackageDialog = true;

			if ( ImGui::MenuItem("Open Package") )
			{
			
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if ( bNewPackageDialog )
	{
		bNewPackageDialog = false;
		ImGui::OpenPopup("NewPackage");
	}

	if ( ImGui::BeginPopupModal("NewPackage"
			, nullptr
			, ImGuiWindowFlags_NoResize ) )
	{
		static char packageName[64] = "unnamedPackage";

		ImGui::PushItemWidth(200);
		ImGui::Text("Name this package:");

		if ( ImGui::InputText("", packageName, C_ARRAYSIZE(packageName)
			, ImGuiInputTextFlags_EnterReturnsTrue 
			| ImGuiInputTextFlags_AutoSelectAll) )
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopItemWidth();


		if ( ImGui::Button("Create") )
			ImGui::CloseCurrentPopup();

		ImGui::SameLine();

		if ( ImGui::Button("Cancel") )
			ImGui::CloseCurrentPopup();

		

		ImGui::EndPopup();
	}

	int32 sizeX = width / 2;
	int32 sizeY = height;
	int32 posX = width - sizeX;

	ImGui::SetNextWindowPos(ImVec2(posX,20.f));
	ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY - 20));

	if ( ImGui::Begin("ArcadeTool", nullptr
		, ImGuiWindowFlags_NoMove 
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar)
		)
	{
		if ( loadedPackage )
		{
			ImGui::Text("%s:", *loadedPackage->getPackageName());
			ImGui::Separator();

			const IMetaData* data = loadedPackage->getMetaData();
			for ( uint32 i=0; i<data->getNumKeys(); ++i )
			{
				ImGui::Text("%s: %s", *data->getKey(i), *data->getValue(i));
			}
			ImGui::Separator();

			for ( uint32 i=0; i<loadedPackage->getNumAssets(); ++i )
			{
				IAsset* asset = loadedPackage->getAsset(i);
				ImGui::Button("|");
				ImGui::SameLine();
				ImGui::Text("%s { %s }", *asset->getName(), asset->getUniqueID().tostring());
			}
		}

		ImGui::End();
	}

	//ImGui::End();
}

static const CString testPackage = "foohy";

static bool buildGamePackage(IPackageManager* packmanager)
{
	shared_ptr<CCompiler> assetCompiler = make_shared<CCompiler>();

	CPackageBuilder* builder = packmanager->createPackageBuilder(testPackage);
	builder->load();

	builder->setAssetCompiler(assetCompiler.get());
	builder->getMetaData()->setKeyValuePair("name", "<unnamed package>");
	builder->getMetaData()->setKeyValuePair("game", "<unnamed game>");
	builder->getMetaData()->setKeyValuePair("author", "anonymous");

	bool success = false;
	if ( !builder->setAndBuildMainScript("foohy.lua") )
	{
		std::cout << "Failed to build" << std::endl;
	}
	else
	{
		std::cout << "Build OK" << std::endl;
		success = true;
	}

	if ( builder->save() )
	{
		std::cout << "Saved OK" << std::endl;
	}
	else
	{
		std::cout << "Failed to save!" << std::endl;
		success = false;
	}

	packmanager->deletePackageBuilder(builder);
	return success;
}

static int start(int argc, char *argv[])
{
	shared_ptr<NativeEnv> native = make_shared<NativeEnv>();
	shared_ptr<CProjectManager> projectManager = make_shared<CProjectManager>(native, "E:/Projects/metacade/projects"); //"../../projects");
	IRuntime *system;

	//PROJECT STUFF
	vector<CProject> projects;
	projectManager->enumerateProjectFolders(projects);

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
	packmanager->setRootDirectory("E:/Projects/metacade/bin/Release");//".");

	buildGamePackage(packmanager);

	if ( initOpenGLAndWindow() ) return 1;

	renderer = make_shared<CRendererGL>();
	renderer->reshape(1280, 720);

	float width = 1280.f;
	float height = 720.f;

	packmanager->findAndPreloadPackages();

	IPackage* defaultPackage = packmanager->getPackageByName(testPackage); //glyphtest
	IGameClass* gameClass = system->getGameClassForPackage(defaultPackage);
	IGameClass* gameClass2 = system->getGameClassForPackage( packmanager->getPackageByName("glyphtest") );

	if ( gameClass == nullptr )
	{
		return onError("Couldn't create game class");
	}

	IGameInstance* gameInstance = nullptr;
	if ( !gameClass->createInstance(&gameInstance) )
	{
		return onError("Couldn't create game instance");
	}

	IGameInstance* gameInstance2 = nullptr;
	if ( !gameClass2->createInstance(&gameInstance2) )
	{
		return onError("Couldn't create other game instance");
	}

	loadedPackage = defaultPackage;
	loadedGameClass = gameClass;

	gameInstance->initializeRenderer(renderer.get());
	gameInstance2->initializeRenderer(renderer.get());

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
			ImGui_ImplSdlGL3_ProcessEvent(&evt);

			if ( evt.type == SDL_QUIT ) 
			{
				running = false;
				break;
			}

			if ( evt.type == SDL_WINDOWEVENT )
			{
				if ( evt.window.event == SDL_WINDOWEVENT_RESIZED )
				{
					renderer->reshape(evt.window.data1, evt.window.data2);
					width = evt.window.data1;
					height = evt.window.data2;
				}
			}

			if ( evt.type == SDL_KEYDOWN )
			{
				if ( evt.key.keysym.sym == SDLK_r )
				{
					if ( gameInstance != nullptr )
					{
						gameInstance->finishRenderer(renderer.get());
						gameClass->deleteInstance(gameInstance);
					}

					//if ( !buildGamePackage(packmanager) ) continue;
					gameClass = system->getGameClassForPackage(packmanager->getPackageByName("glyphtest"));

					gameInstance = nullptr;
					if ( gameClass->createInstance(&gameInstance) )
					{
						gameInstance->initializeRenderer(renderer.get());
					}
				}
			}
		}

		ImGui_ImplSdlGL3_NewFrame(window);

		renderer->clear();

		float time = (float)(SDL_GetTicks()) / 1000.f;
		if (lastTime == 0) lastTime = time;
		deltaSeconds = time - lastTime;
		lastTime = time;

		if ( gameInstance != nullptr ) 
			gameInstance->think(time);

		if ( !paused )
		{
			//for ( uint32 i=0; i<300; ++i )
			{
				if ( gameInstance != nullptr ) 
					gameInstance->render(renderer.get(), CVec2(width/2, height - 20));
			}

			/*if ( gameInstance2 != nullptr )
			{
				gameInstance2->think(time);
				gameInstance2->render(renderer.get(), CVec2(120,120));
			}*/
		}

		immediateUI(width, height);

		ImGui::Render();

		SDL_GL_SwapWindow(window);

		Sleep(5);
	}

	if ( gameInstance != nullptr )
		gameInstance->finishRenderer(renderer.get());

	if ( gameInstance2 != nullptr )
		gameInstance2->finishRenderer(renderer.get());

	if ( gameClass != nullptr )
		gameClass->deleteInstance(gameInstance);

	Arcade::destroy(system);

	ImGui_ImplSdlGL3_Shutdown();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

	_CrtDumpMemoryLeaks();

	return 0;
}

int SDL_main(int argc, char *argv[])
{
	return start(argc, argv);
}
