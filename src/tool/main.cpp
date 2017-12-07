//#include "metacade_public.h"

//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>

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
		snprintf(buf, 4096, "SDL_Error: %s + line: %i", error, line);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", buf, NULL);
		SDL_ClearError();
	}
}

static SDL_Window *window = NULL;
static SDL_GLContext glContext;
static SDL_AudioSpec sndFormat;
static SDL_AudioDeviceID sndDevice;
static SDL_mutex* sndMutex;
static IRuntime* runtime = nullptr;
static IPackage* loadedPackage = nullptr;
static IGameClass* loadedGameClass = nullptr;
static IGameInstance* instance = nullptr;
static IGameInstance* instance2 = nullptr;

static bool demoRecord = false;
static bool demoPlayback = false;
static uint32 demoFrame = 0;

static void sndCallback(void*, uint8* stream, int32 len)
{
	SDL_LockMutex(sndMutex);

	ISoundMixer* mixer = nullptr;
	if ( instance == nullptr || (mixer = instance->getSoundMixer()) == nullptr )
	{
		memset(stream, 0, len*sizeof(uint8));
		SDL_UnlockMutex(sndMutex);
		return;
	}

	mixer->update();

	memcpy(stream, mixer->getPCMSamples(), len);

	SDL_UnlockMutex(sndMutex);
}

#define SOUND_DEVICE_SAMPLES 512

static int initSound()
{
	SDL_AudioSpec sndFormatRequest;
	SDL_memset(&sndFormatRequest, 0, sizeof(sndFormatRequest));

	sndMutex = SDL_CreateMutex();

	sndFormatRequest.freq = 44100;
	sndFormatRequest.format = AUDIO_S16;
	sndFormatRequest.channels = 2;
	sndFormatRequest.samples = SOUND_DEVICE_SAMPLES;
	sndFormatRequest.callback = sndCallback;

	/*int32 numDevices = SDL_GetNumAudioDevices(0);

	for ( int32 i=0; i<numDevices; ++i )
	{
		std::cout << i << SDL_GetAudioDeviceName(i, 0) << std::endl;
	}*/

	sndDevice = SDL_OpenAudioDevice(NULL, 0, &sndFormatRequest, &sndFormat, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if ( sndDevice == 0 )
	{
		return onError("Unable to create sound device");
	}
	else
	{
		if ( sndFormat.format != sndFormatRequest.format )
		{
			return onError("Unable to get desired sample type (signed 16)");
		}
		SDL_PauseAudioDevice(sndDevice, 0);

		std::cout << "SAMPLES AVAILABLE: " << sndFormatRequest.samples << " --> " << sndFormat.samples << std::endl;
	}

	return 0;
}

static void shudownSound()
{
	SDL_CloseAudioDevice(sndDevice);
	SDL_DestroyMutex(sndMutex);
}

static int initOpenGLAndWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_EVENTS);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);  //24-bit depth buffer

	//Disabling multi-sampling because samples miss geometry in weird ways causing line artifacts
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); //4x multi-sampling for pretty lines

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) return onError("Failed To Init SDL_Window");


	checkError(__LINE__);
	glContext = SDL_GL_CreateContext(window);
	checkError(__LINE__);
	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		return onError((const char*) glewGetErrorString(err));
	}

	ImGui_ImplSdlGL3_Init(window);

	return 0;
}

struct CUIState
{
	float fUIRolloutFraction;
	bool bShowInfo;
	bool bNewPackageDialog;
	uint32 memUsage;

	CUIState()
		: fUIRolloutFraction(1.f)
		, bShowInfo(true)
		, bNewPackageDialog(false)
		, memUsage(0)
	{}
};

static CUIState g_UIState;

static const float UI_SPLIT = .7f;

static void immediateUI(float width, float height, float deltatime)
{
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu("File") )
		{
			if ( ImGui::MenuItem("New Package") ) 
				g_UIState.bNewPackageDialog = true;

			if ( ImGui::MenuItem("Open Package") )
			{
			
			}
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu("View") )
		{
			ImGui::MenuItem("Show Info", NULL, &g_UIState.bShowInfo, true);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if ( g_UIState.bShowInfo )
	{
		g_UIState.fUIRolloutFraction = min( g_UIState.fUIRolloutFraction + deltatime * 2.f, 1.f );
	}
	else
	{
		g_UIState.fUIRolloutFraction = max( g_UIState.fUIRolloutFraction - deltatime * 2.f, 0.f );
	}

	if ( g_UIState.bNewPackageDialog )
	{
		g_UIState.bNewPackageDialog = false;
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

	if ( g_UIState.fUIRolloutFraction > 0.f )
	{

		float sizeX = width * (1.f - UI_SPLIT);
		float sizeY = height;
		float posX = width - sizeX * g_UIState.fUIRolloutFraction;

		ImGui::SetNextWindowPos(ImVec2(posX,20.f));
		ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY - 20.f));

		if ( ImGui::Begin("ArcadeTool", nullptr
			, ImGuiWindowFlags_NoMove 
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoTitleBar)
			)
		{
			ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f), "Mem %ikb", g_UIState.memUsage);

			if ( demoPlayback )
			{
				ImGui::TextColored(ImVec4(0.f,1.f,0.f,1.f), "PLAY DEMO frame %i", demoFrame);
			}

			if ( demoRecord )
			{
				ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f), "RECORD DEMO frame %i", demoFrame);
			}

			if ( loadedPackage )
			{
				ImGui::TextWrapped("%s:", *loadedPackage->getPackageName());
				ImGui::Separator();

				const IMetaData* data = loadedPackage->getMetaData();
				for ( uint32 i=0; i<data->getNumKeys(); ++i )
				{
					ImGui::TextWrapped("%s: %s", *data->getKey(i), *data->getValue(i).toString());
				}
				ImGui::Separator();
				ImGui::Text("Assets:");
				for ( uint32 i=0; i<loadedPackage->getNumAssets(); ++i )
				{
					IAsset* asset = loadedPackage->getAsset(i).get();
					//ImGui::Button("|");
					//ImGui::SameLine();
					ImGui::Text("  %s", *asset->getName());
				}
			}

			ImGui::End();
		}

	}

	//ImGui::End();
}

typedef vector<CInputEvent> EventBuffer;

void processSDLInputs(const SDL_Event& evt, CInputState& baseline, EventBuffer& events)
{
	if ( evt.type == SDL_MOUSEMOTION )
	{
		CInputState state;
		state.setMousePosition((float) evt.motion.x, (float) evt.motion.y - 20.f);
		state.setMouseIsFocused(evt.motion.x > 0 && evt.motion.x < 400 && evt.motion.y > 20 && evt.motion.y < 320);
		state.generateEvents(baseline, [&events](const CInputEvent &ev) mutable
		{
			events.push_back(ev);
		});
		baseline.merge(state);
	}

	if ( evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP )
	{
		CInputState state;
		uint32 mouseButtons = SDL_GetMouseState(NULL, NULL);
		state.setMouseButton(EMouseButton::MOUSE_BUTTON_LEFT, (mouseButtons & SDL_BUTTON_LMASK) != 0);
		state.setMouseButton(EMouseButton::MOUSE_BUTTON_RIGHT, (mouseButtons & SDL_BUTTON_RMASK) != 0);
		state.setMouseButton(EMouseButton::MOUSE_BUTTON_MIDDLE, (mouseButtons & SDL_BUTTON_MMASK) != 0);
		state.setMouseButton(EMouseButton::MOUSE_BUTTON_X1, (mouseButtons & SDL_BUTTON_X1MASK) != 0);
		state.setMouseButton(EMouseButton::MOUSE_BUTTON_X2, (mouseButtons & SDL_BUTTON_X2MASK) != 0);
		state.generateEvents(baseline, [&events](const CInputEvent &ev) mutable
		{
			events.push_back(ev);
		});
		baseline.merge(state);
	}

	if ( evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP )
	{
		CInputEvent ev = CInputEvent::generateKeyEvent((uint8) evt.key.keysym.scancode, evt.type == SDL_KEYDOWN);
		events.push_back(ev);
		baseline.applyEvent(ev);
	}
}

static void testCurrent(const char* name, Arcade::IRuntime* rt)
{
	if ( rt == nullptr ) { std::cout << "Runtime[" << name << "] is not valid" << std::endl; return; }
	std::cout << "Runtime[" << name << "] is " << (rt->isCurrent() ? "current" : "NOT current") << std::endl; 
}

static void innerThread(const char* name, Arcade::IRuntime* rt)
{
	std::cout << "TEST CURRENT IN THREAD" << std::endl;
	rt->makeCurrent();
	testCurrent(name, rt);
}

static int threadTest()
{
	Arcade::IRuntime *rtA = nullptr;
	Arcade::IRuntime *rtB = nullptr;
	Arcade::create(&rtA);
	Arcade::create(&rtB);

	rtA->makeCurrent();

	std::cout << "PRE TEST CURRENT" << std::endl;
	testCurrent("rtA", rtA);
	testCurrent("rtB", rtB);

	std::thread t(innerThread, "rtB-thread", rtB);
	t.join();

	std::cout << "POST TEST CURRENT" << std::endl;
	testCurrent("rtA", rtA);
	testCurrent("rtB", rtB);

	return 0;
}

static int start(int argc, char *argv[])
{
	uint32 nextAvailableDemoFrame = 0;
	CString runPackage;

	//if ( true ) return threadTest();

	if ( argc < 2 ) 
	{
		std::cout << "Usage: tool <project name>" << std::endl;	
		return 0;
	}

	if ( argc > 1 )
	{
		CString xarg(argv[1]);
		if ( xarg.endsWith(".mpkg") )
		{
			runPackage = xarg;
		}
	}

	if ( argc > 2 )
	{
		CString xarg(argv[2]);
		if ( xarg == "-record" ) demoRecord = true;
		if ( xarg == "-play" ) demoPlayback = true;
	}

	shared_ptr<NativeEnv> native = make_shared<NativeEnv>();

#ifdef DEBUG
	shared_ptr<CProjectManager> projectManager = make_shared<CProjectManager>(native, "../../projects");
	std::cout << "../../projects" << std::endl;
#else
	shared_ptr<CProjectManager> projectManager = make_shared<CProjectManager>(native, "projects");
	std::cout << "projects" << std::endl;
#endif

	IFileObject* demoFile = nullptr;

	if ( demoRecord )
	{
		demoFile = native->getFileSystem()->openFile("demo.dat", FILE_WRITE);
	}
	else if ( demoPlayback )
	{
		demoFile = native->getFileSystem()->openFile("demo.dat", FILE_READ);
		demoFile->read(&nextAvailableDemoFrame, sizeof(uint32));
	}

	if ( Arcade::create(&runtime) && runtime->initialize(native.get()) )
	{
		cout << "Initialized" << endl;
	}
	else
	{
		return onError("Failed to init arcade runtime");
	}

	std::cout << "Loading Packages..." << std::endl;
	IPackageManager* packmanager = runtime->getPackageManager();
	packmanager->setRootDirectory(".");
	packmanager->findAndPreloadPackages();

	shared_ptr<CProject> targetProject = nullptr;

	if ( runPackage.empty() )
	{
		//PROJECT STUFF
		vector<CProject> projects;
		projectManager->enumerateProjectFolders(projects);

		for ( CProject& p : projects )
		{
			if ( p.getProjectName() == CString(argv[1]) )
				targetProject = make_shared<CProject>(p);
		}

		//projectManager->saveProjectAs(targetProject, "killer.mproject");

		if ( targetProject == nullptr )
		{
			std::cout << "No project" << std::endl;
			Arcade::destroy(runtime);
			return 0;
		}

		IPackage* pkg = targetProject->buildPackage(runtime);
		loadedPackage = pkg;
	}
	else
	{
		loadedPackage = packmanager->getPackageByName(runPackage.chopRight(5));

		if ( loadedPackage == nullptr )
		{
			std::cout << "Unable to load package " << runPackage.get() << std::endl;
			return 0;
		}
	}

	if ( initOpenGLAndWindow() || initSound() ) return 1;

	shared_ptr<CRendererGL> renderer = make_shared<CRendererGL>();
	renderer->reshape(1280, 720);

	CMixerSettings mixerSettings;
	mixerSettings.bufferSize = SOUND_DEVICE_SAMPLES;
	mixerSettings.sampleRate = 44100;
	mixerSettings.maxChannels = 64;

	{
		loadedGameClass = runtime->getGameClassForPackage(loadedPackage);

		if ( loadedGameClass != nullptr && loadedGameClass->createInstance(&instance) )
		{
			instance->initializeRenderer(renderer.get());
			instance->initSoundMixer(mixerSettings);
			instance->init();
		}

		/*if ( loadedGameClass != nullptr && loadedGameClass->createInstance(&instance2) )
		{
			instance2->initializeRenderer(renderer.get());
			instance2->init();
		}*/
	}


	float width = 1280.f;
	float height = 720.f;
	bool running = true;
	bool fastForward = false;
	float lastTime = 0;
	float deltaSeconds = 0;
	float fixedTime = 0;
	float fakeTime = 0;
	EventBuffer events;
	CInputState baseline;
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
					width = (float) evt.window.data1;
					height = (float) evt.window.data2;
				}
			}

			if ( evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP )
			{
				if ( evt.key.keysym.sym == SDLK_F1 )
				{
					fastForward = evt.type == SDL_KEYDOWN;
					continue;
				}
			}

			if ( !demoPlayback ) processSDLInputs(evt, baseline, events);

			if ( evt.type == SDL_KEYDOWN )
			{
				if ( evt.key.keysym.sym == SDLK_r )
				{
					SDL_LockMutex(sndMutex);
					if ( instance != nullptr )
					{
						instance->finishRenderer(renderer.get());
						loadedGameClass->deleteInstance(instance);
						instance = nullptr;
					}
					SDL_UnlockMutex(sndMutex);

					if ( targetProject != nullptr )
					{
						loadedPackage = targetProject->buildPackage(runtime);
					}
					loadedGameClass = runtime->getGameClassForPackage(loadedPackage);

					SDL_LockMutex(sndMutex);
					if ( loadedGameClass->createInstance(&instance) )
					{
						instance->initializeRenderer(renderer.get());
						instance->initSoundMixer(mixerSettings);
						instance->init();
					}
					SDL_UnlockMutex(sndMutex);
				}
			}
		}

		ImGui_ImplSdlGL3_NewFrame(window);

		renderer->clear();

		float time = (float)(SDL_GetTicks()) / 1000.f;
		if (lastTime == 0) lastTime = time;
		deltaSeconds = time - lastTime;
		lastTime = time;

		fakeTime = fakeTime + deltaSeconds;

		if ( fastForward )
		{
			fakeTime = fakeTime + deltaSeconds * 2.f;
		}

		while ( fixedTime < fakeTime )
		{
			if ( demoRecord || demoPlayback )
			{
				++demoFrame;
			}

			uint8 numEvents = (uint8) (events.size() & 0xFF);
			if ( demoRecord && numEvents > 0 )
			{
				demoFile->write( &demoFrame, sizeof(uint32) );
				demoFile->write( &numEvents, sizeof(uint8) );

				for ( uint32 i=0; i<numEvents; ++i )
				{
					CInputEvent& ev = events[i];
					demoFile->write( &ev, sizeof(CInputEvent) );
				}
			}

			if ( demoPlayback && nextAvailableDemoFrame == demoFrame )
			{
				demoFile->read( &numEvents, sizeof(uint8) );

				for ( uint32 i=0; i<numEvents; ++i )
				{
					CInputEvent ev = CInputEvent::generateKeyEvent(0,false);
					demoFile->read( &ev, sizeof(CInputEvent) );
					events.push_back(ev);
				}

				if ( !demoFile->read(&nextAvailableDemoFrame, sizeof(uint32)) )
				{
					demoPlayback = false;
				}
			}

			for ( CInputEvent& ev : events )
			{
				if ( instance != nullptr) instance->postInputEvent(ev);
				//instance2->postInputEvent(ev);
			}
			events.clear();

			fixedTime += 1.f / 60.f;
			if ( instance != nullptr ) instance->think(fixedTime);
			//if ( instance2 != nullptr ) instance2->think(fixedTime);
		}

		if ( instance != nullptr ) 
		{
			float w = (width * UI_SPLIT) * g_UIState.fUIRolloutFraction + width * (1.f - g_UIState.fUIRolloutFraction);
			instance->render(renderer.get(), CVec2(w,height - 20.f));
		}

		//renderer->setOffset(CVec2(0,300.f));

		/*if ( instance2 != nullptr ) 
		{
			instance2->render(renderer.get(), CVec2(400,300));
		}*/

		//renderer->setOffset(CVec2(0,0.f));

		g_UIState.memUsage = (uint32) (native->getMemUsage() >> 10);

		immediateUI(width, height, deltaSeconds);

		ImGui::Render();

		SDL_GL_SwapWindow(window);

		Sleep(5);
	}

	shudownSound();

	if ( loadedGameClass != nullptr && instance != nullptr )
	{
		instance->finishRenderer(renderer.get());
		loadedGameClass->deleteInstance( instance );
	}

	if ( demoFile != nullptr )
	{
		native->getFileSystem()->closeFile(demoFile);
	}

	Arcade::destroy(runtime);

	ImGui_ImplSdlGL3_Shutdown();

	renderer.reset();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);

	SDL_Quit();

	//_CrtDumpMemoryLeaks();

	return 0;
}

int SDL_main(int argc, char *argv[])
{
	//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Mem", "Take a baseline snapshot", NULL);

	int ret = start(argc, argv);

	for(int i=0; i<argc; ++i)
		if ( !strcmp(argv[i], "-test") )
			std::cout << "METACADE_EXIT_SUCCESS" << std::endl;


	//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Mem", "Take final snapshot", NULL);
	return ret;
}
