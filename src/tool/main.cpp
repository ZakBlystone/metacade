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
static SDL_AudioSpec sndFormat;
static SDL_AudioDeviceID sndDevice;
static shared_ptr<CRendererGL> renderer;
static IRuntime* runtime = nullptr;
static IPackage* loadedPackage = nullptr;
static IGameClass* loadedGameClass = nullptr;
static ISoundMixer* mixer = nullptr;
static CAssetRef testSound;
static CAssetRef testSound2;
static CAssetRef testSound3;

static uint32 streamOffset = 0;
static void sndCallback(void* userdata, uint8* stream, int32 len)
{
	if ( mixer == nullptr ) return;
	mixer->update();

	memcpy(stream, mixer->getPCMSamples(), len);

	streamOffset += len;
}

static int initSound()
{
	SDL_AudioSpec sndFormatRequest;
	SDL_memset(&sndFormatRequest, 0, sizeof(sndFormatRequest));

	sndFormatRequest.freq = 44100;
	sndFormatRequest.format = AUDIO_S16;
	sndFormatRequest.channels = 2;
	sndFormatRequest.samples = 512;
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
	}

	return 0;
}

static void shudownSound()
{
	SDL_CloseAudioDevice(sndDevice);
}

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
				IAsset* asset = loadedPackage->getAsset(i).get(runtime);
				ImGui::Button("|");
				ImGui::SameLine();
				ImGui::Text("%s { %s }", *asset->getName(), asset->getUniqueID().tostring());
			}
		}

		ImGui::End();
	}

	//ImGui::End();
}

static int start(int argc, char *argv[])
{
	shared_ptr<NativeEnv> native = make_shared<NativeEnv>();
	shared_ptr<CProjectManager> projectManager = make_shared<CProjectManager>(native, "E:/Projects/metacade/projects"); //"../../projects");
	IGameInstance* instance = nullptr;

	if ( Arcade::create(&runtime) && runtime->initialize(native.get()) )
	{
		cout << "Initialized" << endl;
	}
	else
	{
		return onError("Failed to init arcade runtime");;
	}

	CMixerSettings mixerSettings;
	mixerSettings.bufferSize = 512;
	mixerSettings.sampleRate = 44100;
	mixerSettings.maxChannels = 64;
	//mixer = runtime->createSoundMixer(mixerSettings);

	std::cout << "Loading Packages..." << std::endl;
	IPackageManager* packmanager = runtime->getPackageManager();
	packmanager->setRootDirectory("E:/Projects/metacade/bin/Release");//".");

	//PROJECT STUFF
	vector<CProject> projects;
	projectManager->enumerateProjectFolders(projects);
	projectManager->saveProjectAs(projects[0], "killer.mproject");

	std::cout << *projects[1].getProjectName() << std::endl;

	IPackage* pkg = projects[1].buildPackage(runtime);
	loadedPackage = pkg;

	if ( pkg != nullptr && pkg->loadAssets() )
	{
		testSound = pkg->findAssetByName("winmusic.snd");
		testSound2 = pkg->findAssetByName("chime.snd");
		testSound3 = pkg->findAssetByName("pop.snd");
	}

	if ( initOpenGLAndWindow() || initSound() ) return 1;

	/*int8 flip = 0x80;
	for ( uint8 i = 0; i < 0xFF; ++i )
	{
		std::cout << (int32) (*(int8*)(&i) ^ flip) << std::endl;
	}*/

	renderer = make_shared<CRendererGL>();
	renderer->reshape(1280, 720);


	{
		loadedGameClass = runtime->getGameClassForPackage(pkg);

		uint32 preInstance = SDL_GetTicks();

		if ( loadedGameClass != nullptr && loadedGameClass->createInstance(&instance) )
		{
			instance->initializeRenderer(renderer.get());
			instance->initSoundMixer(mixerSettings);
			mixer = instance->getSoundMixer();


			/*{
				int32 chan = mixer->playSound(testSound);
				mixer->setChannelLooping(chan, true);
				mixer->setChannelVolume(chan, 0.1f);
				mixer->setChannelPitch(chan, 1.0f);
			}*/
		}

		uint32 instanceCreationTime = SDL_GetTicks() - preInstance;

		std::cout << "INSTANCE CREATION TOOK: " << instanceCreationTime << "ms" << std::endl;
	}


	float width = 1280.f;
	float height = 720.f;

	packmanager->findAndPreloadPackages();

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

			if ( evt.type == SDL_MOUSEMOTION )
			{
				CInputState state;
				state.setMousePosition(evt.motion.x, evt.motion.y - 20);
				state.setMouseIsFocused(evt.motion.x > 0 && evt.motion.x < 400 & evt.motion.y > 20 && evt.motion.y < 320);
				instance->postInputState(state);
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
				instance->postInputState(state);
			}

			if ( evt.type == SDL_KEYDOWN )
			{
				if ( evt.key.keysym.sym == SDLK_x )
				{
					int32 chan = mixer->playSound(testSound2);
					mixer->setChannelPitch( chan, 1.2f );
				}
				if ( evt.key.keysym.sym == SDLK_c )
				{
					mixer->playSound(testSound3);
				}
				if ( evt.key.keysym.sym == SDLK_r )
				{
					if ( instance != nullptr )
					{
						instance->finishRenderer(renderer.get());
						loadedGameClass->deleteInstance(instance);
					}

					//if ( !buildGamePackage(packmanager) ) continue;
					//loadedGameClass = system->getGameClassForPackage(packmanager->getPackageByName("glyphtest"));
					loadedPackage = projects[1].buildPackage(runtime);
					loadedGameClass = runtime->getGameClassForPackage(loadedPackage);

					instance = nullptr;
					if ( loadedGameClass->createInstance(&instance) )
					{
						instance->initializeRenderer(renderer.get());
					}

					{
						int32 chan = mixer->playSound(testSound);
						mixer->setChannelLooping(chan, true);
						mixer->setChannelVolume(chan, 0.1f);
						mixer->setChannelPitch(chan, 1.0f);
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

		if ( instance != nullptr ) 
		{
			instance->think(time);
			instance->render(renderer.get(), CVec2(400,300));
		}

		if ( !paused )
		{
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

	shudownSound();

	//runtime->deleteSoundMixer(mixer);

	Arcade::destroy(runtime);

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
