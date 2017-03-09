//#include "metacade_public.h"
#include <iostream>
#include <memory>

using std::ostream;
using std::istream;
using std::shared_ptr;
using std::make_shared;

#include "metacade_amal.h"

using namespace Arcade;

#include "SDL.h"
#include "GL/glew.h"
#include "GL/glew.c"
#include "IL/il.h"

#include "glrender.h"
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

int start(int argc, char *argv[])
{
	ilInit();

	/*ILuint test = ilGenImage();
	ilBindImage(test);
	ilLoadImage("E:/Temp/chair_icon0.png");
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int Width = (int) ilGetInteger(IL_IMAGE_WIDTH);
	int Height = (int) ilGetInteger(IL_IMAGE_HEIGHT);
	
	uint8* Data = (uint8*) ilGetData();

	std::fstream output("E:/Temp/pic3.dat", std::ios::out | std::ios::binary);

	output.write((const char *)& Width, 4);
	output.write((const char *)& Height, 4);
	output.write((const char *)Data, Width * Height * 4);

	output.close();*/

	if ( initOpenGLAndWindow() ) return 1;
	if ( !CRuntime::initialize() ) return onError("Failed to init arcade runtime");

	renderer = make_shared<CRendererGL>();
	renderer->reshape(800, 600);

	CRuntime::testRenderStart(renderer.get());

	bool running = false;
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
		}

		float time = (float)(SDL_GetTicks()) / 1000.f;
		if (lastTime == 0) lastTime = time;
		deltaSeconds = time - lastTime;
		lastTime = time;

		//glClearColor(0.1f, 0.1f, 0.2f, 1.0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CRuntime::testRendering(renderer.get(), time, CVec2(400, 300));
		
		SDL_GL_SwapWindow(window);

		Sleep(5);
	}

	CRuntime::testRenderEnd(renderer.get());

	ilShutDown();

	CRuntime::shutdown();

	return 0;
}

int SDL_main(int argc, char *argv[])
{
	return start(argc, argv);
}
