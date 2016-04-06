/*
* <Game.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Game.h"
#include "GL/glew.h"
#include "Package.h"
#include "AssetTypes.h"
#include "GLContextHost.h"
#include "GLRenderThread.h"
#include "RenderStack.h"
#include "LuaVM.h"

#include <map>
#include <string>
#include <algorithm>

using namespace Arcade;

//Map for correlating strings to controller button states
typedef std::map<std::string, ControlEventState> ControlStateBufferMap;

//
//OpenGL Renderer callbacks
//
class RenderCalls : public GLRenderThread::ICallbacks
{
public:
	Game *HostGame; //Pointer to Game instance
	RenderCalls(Game *G) : HostGame(G) {}

	//Called when renderer starts up
	virtual void Init()
	{
		//Enable texturing and blending
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		//Obtain size of frame buffer
		int W, H;
		HostGame->GetRenderSize(W, H);

		//Set orthographic projection mode
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, W, 0, H, 0, 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	//Called when a frame is to be drawn
	virtual void Draw()
	{
		//Reset modelview matrix
		glLoadIdentity();

		//Set default blend function (alpha blending)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Set default color (white)
		glColor4f(1.f, 1.f, 1.f, 1.f);

		//Execute drawing commands in render stack
		HostGame->GetRenderStack()->Execute();
	}

	//Called when renderer is destroyed
	virtual void Shutdown()
	{
	}
};

//
//Game constructor:
//	ContextHost is the main OpenGL context, this is used to start the renderthread
//	Package is the game package to run in this instance
//
Game::Game(GLContextHost *ContextHost, GamePackage *Package) :
	GameTime(0.f), 
	PreviousDT(0.f)
{
	//Hang on to the package
	this->Package = Package;

	//Instantiate a new control state buffer map
	this->ControlStateBuffer = new ControlStateBufferMap;

	//Upload assets to OpenGL (if they're not uploaded already)
	Package->UploadNativeAssets();

	//Create instance of render callbacks
	RenderCallbacks = new RenderCalls(this);

	//Create render stack
	Render = new RenderStack();

	Runtime::LogPrint(LOG_MESSAGE, "Starting render thread...");

	//Create render thread
	RenderThread = new GLRenderThread(256, 256, (RenderCalls*) RenderCallbacks);
	RenderThread->Init(ContextHost);

	Runtime::LogPrint(LOG_MESSAGE, "Render thread is ready.");
	Runtime::LogPrint(LOG_MESSAGE, "Starting LuaVM...");

	//Create Lua virtual machine
	VM = new LuaVM;
	VM->Init();

	Runtime::LogPrint(LOG_MESSAGE, "LuaVM is ready.");
	Runtime::LogPrint(LOG_MESSAGE, "Caching game assets...");

	//Execute main.lua from the Package
	CodeAsset *Code = CastAsset<CodeAsset>(Package->GetAssetByName("__luamain"));
	if (Code)
	{
		VM->InstallMath();			//Install math functions
		VM->InstallCode(Code);		//Execute the code buffer
		VM->InstallRuntime(this);	//Install the runtime
	}

	//Execute the 'cache' function in the Lua virtual machine
	if (!VM->SetFunction("cache") || !VM->Call())
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to execute game cache function.");
	}

	Runtime::LogPrint(LOG_MESSAGE, "Game assets cached.");
}

//
//Game destructor
//
Game::~Game()
{
	//Shut down render thread if it exists
	if (RenderThread)
	{
		RenderThread->Shutdown();
	}

	//Close the Lua virtual machine
	VM->Close();

	//Delete resources
	delete RenderThread;
	delete (RenderCalls*)RenderCallbacks;
	delete Render;
	delete VM;

	//Nullify renderthread pointer
	RenderThread = nullptr;

	//Delete control buffer
	ControlStateBufferMap *Controls = (ControlStateBufferMap *) this->ControlStateBuffer;
	if (Controls)
	{
		delete Controls;
	}
}

//Return size of RenderThread buffer
void Game::GetRenderSize(int &Width, int &Height)
{
	Width = RenderThread->GetBuffer()->GetWidth();
	Height = RenderThread->GetBuffer()->GetHeight();
}

//Return true if RenderThread buffer has finished rendering the requested frame
bool Game::FrameIsPresentable()
{
	return RenderThread->FrameIsPresentable();
}

//Get bitmap from RenderThread
char * Game::GetFrameBufferRGB24Bit()
{
	return (char *) RenderThread->GetBuffer()->GetBitmap();
}

//Not implemented yet
void Game::GetAudioSamples16BitMono(unsigned short *Stream, int DesiredSamples)
{
}

//Run an update tick on the arcade VM, advances game time by 'DeltaTime'
void Game::Update(float DeltaTime)
{
	PreviousDT = DeltaTime;
	GameTime += DeltaTime;

	//Attempt to load function 'loop'
	if (VM->SetFunction("loop"))
	{
		//Push GameTime and PreviousDT onto the stack
		VM->Push(GameTime);
		VM->Push(PreviousDT);

		//Call the function
		if (!VM->Call()) Runtime::LogPrint(LOG_ERROR, "Unable to execute game update loop.");
	}
	else
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to execute game update loop.");
	}

	//Update control buffer
	//Set PRESSED events to DOWN events
	//Set RELEASED events to UP events
	ControlStateBufferMap *Buffer = (ControlStateBufferMap *)ControlStateBuffer;
	for (auto iter = Buffer->begin(); iter != Buffer->end(); ++iter)
	{
		if ((*iter).second == CSTATE_PRESSED) (*iter).second = CSTATE_DOWN;
		if ((*iter).second == CSTATE_RELEASED) (*iter).second = CSTATE_UP;
	}
}

void Game::AdvanceFrame()
{
	//If the last requested frame is not ready, don't do anything yet
	if (FrameIsPresentable())
	{
		//Clear the Render stack
		Render->Clear();

		//Attempt to load function 'draw'
		if (VM->SetFunction("draw"))
		{
			//Push GameTime and PreviousDT onto the stack
			VM->Push(GameTime);
			VM->Push(PreviousDT);

			//Call the function
			if (!VM->Call()) Runtime::LogPrint(LOG_ERROR, "Unable to execute game draw loop.");
		}
		else
		{
			Runtime::LogPrint(LOG_ERROR, "Unable to execute game draw loop.");
		}

		//Tell the render thread to render a frame
		RenderThread->AdvanceFrame();
	}
}

//GamePackage for this arcade instance
GamePackage * Game::GetPackage()
{
	return Package;
}

//Render stack for this arcade instance
RenderStack * Game::GetRenderStack()
{
	return Render;
}

//Send a controller button event to the arcade VM
void Arcade::Game::ControllerEvent(const char *InputName, bool IsDown)
{
	//Cast ControlStateBuffer pointer to a ControlStateBufferMap so that it can be used
	ControlStateBufferMap *Buffer = (ControlStateBufferMap *)ControlStateBuffer;

	//Make the InputName lowercase
	std::string Name(InputName);
	std::transform(Name.begin(), Name.end(), Name.begin(), ::tolower);

	//Update the state of the control
	(*Buffer)[Name] = IsDown ? CSTATE_PRESSED : CSTATE_RELEASED;

	Runtime::LogPrint(LOG_MESSAGE, "KEY: %s %i", Name.c_str(), IsDown);
}

//Get the state of a controller button
Arcade::ControlEventState Arcade::Game::GetControllerEventState(const char *InputName)
{
	//Cast ControlStateBuffer pointer to a ControlStateBufferMap so that it can be used
	ControlStateBufferMap *Buffer = (ControlStateBufferMap *)ControlStateBuffer;

	//Find the control state, if the control isn't found it's assumed to be un-pressed
	auto iter = Buffer->find(InputName);
	if (iter == Buffer->end()) return CSTATE_UP;

	//Return the control state
	return (*iter).second;
}

//Lua virtual machine for this arcade instance
LuaVM * Arcade::Game::GetLuaVM()
{
	return VM;
}

//Get the averaged color of the frame buffer
void Arcade::Game::GetAverageFrameBufferColor(float Color[3])
{
	//Initialize color to black
	Color[0] = Color[1] = Color[2] = 0.f;
	int W, H;
	
	//Get screen frame buffer
	unsigned char *Screen = (unsigned char *) GetFrameBufferRGB24Bit();

	//Get size of frame buffer
	GetRenderSize(W, H);

	//Conversion from (0-255) to (0-1)
	const float InvClr = 1.f / 255.f;

	//Iterate over all the pixels and add their values to 'Color'
	for (int i = 0; i < W*H*3; i+=3)
	{
		unsigned char *Pix = Screen + i;

		Color[0] += (float)(Pix[0]) * InvClr;
		Color[1] += (float)(Pix[1]) * InvClr;
		Color[2] += (float)(Pix[2]) * InvClr;
	}

	//Divide by number of pixels to obtain the average color
	int Pixels = W * H;
	Color[0] /= Pixels;
	Color[1] /= Pixels;
	Color[2] /= Pixels;
}

