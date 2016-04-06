/*
* <Main.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <Windows.h>
#include "LuaVM.h"
#include "SDL.h"
#include "GL/glew.h"
#include "GL/glew.c"
#include "IL/il.h"
#include "RenderStack.h"

#include "Runtime.h"
#include "AssetTypes.h"
#include "Package.h"
#include "PackageManager.h"
#include "Game.h"

using namespace Arcade;

class FileObject : public IFileObject
{
public:
	virtual bool Seek(unsigned int Offset) { return fseek(FP, Offset, SEEK_SET) == 0; }
	virtual bool Read(void *Data, unsigned int Bytes) { return fread(Data, 1, Bytes, FP) == Bytes; }
	virtual bool Write(void *Data, unsigned int Bytes) { return fwrite(Data, 1, Bytes, FP) == Bytes; }
	virtual unsigned int Tell() { return ftell(FP); }
	virtual unsigned int GetSize() { return Size; }

	FILE *FP;
	unsigned int Size;
};

static bool IsBuilding = true;

class HostInterface : public 
	IHostInterface, 
	IHostFileInterface, 
	IHostBuildInterface
{
public:
	virtual IFileObject * OpenFile(const char *FileName, FileIOMode Mode)
	{
		FileObject *F = new FileObject;
		F->FP = fopen(FileName, Mode == FILE_READ ? "rb" : "wb");

		if (!F->FP) { delete F; return nullptr; }

		if (Mode == FILE_READ)
		{
			fseek(F->FP, 0, SEEK_END);
			F->Size = ftell(F->FP);
		}
		else
		{
			F->Size = 0;
		}

		fseek(F->FP, 0, SEEK_SET);

		return F;
	}

	virtual void CloseFile(IFileObject *Object)
	{
		FileObject *F = (FileObject *) Object;
		if (!F) return;
		if (F->FP)
		{
			fflush(F->FP);
			fclose(F->FP);
		}
		delete F;
	}

	virtual IHostFileInterface * GetFileInterface() { return this; }
	virtual IHostBuildInterface * GetBuildInterface() { return IsBuilding ? this : nullptr; }

	virtual bool CompileImageAsset(const char *Filename, ImageAsset *Image)
	{
		ILuint Src = ilGenImage();

		ilBindImage(Src);
		if (!ilLoadImage(Filename))
		{
			Runtime::LogPrint(LOG_ERROR, "Unable to load image asset '%s'.", Filename);
			return false;
		}

		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		int Width = (int) ilGetInteger(IL_IMAGE_WIDTH);
		int Height = (int) ilGetInteger(IL_IMAGE_HEIGHT);

		char *Data = (char*) ilGetData();
		Image->SetPixelsRGBA(Data, Width, Height);

		Runtime::LogPrint(LOG_MESSAGE, "Compiled image '%s' %ix%i.", Filename, Width, Height);

		return true;
	}

	virtual bool CompileSoundAsset(const char *Filename, SoundAsset *Sound)
	{
		SDL_AudioSpec Spec;
		Uint8 *WaveBuffer;
		Uint32 WaveLength;
		if (SDL_LoadWAV(Filename, &Spec, &WaveBuffer, &WaveLength) == NULL)
		{
			Runtime::LogPrint(LOG_ERROR, "Unable to load .WAV asset '%s'.", Filename);
			return false;
		}

		if (Spec.channels != 1 || (Spec.format != AUDIO_U16 && Spec.format != AUDIO_S16) || (Spec.freq != 22050) )
		{
			Runtime::LogPrint(LOG_ERROR, "Unable to compile .WAV asset '%s', Wave files must be 22050 16bit mono.", Filename);
			return false;
		}

		if ( Spec.format == AUDIO_U16 ) 
			Runtime::LogPrint(LOG_WARN, ".WAV asset '%s' is unsigned!", Filename);

		Sound->SetWaveSamples((char *)WaveBuffer, Spec.freq, WaveLength/2, Spec.format == AUDIO_S16);

		SDL_FreeWAV(WaveBuffer);

		return true;
	}

};

FILE *LogOutputFile = nullptr;
class ArcadeLog : public IArcadeLogCallback
{
	virtual void LogMessage(const char *Text, LogMessageType &Type)
	{
		const char *Head = "[Msg] ";
		if (Type == LogMessageType::LOG_ERROR) Head = "[Error] ";
		if (Type == LogMessageType::LOG_WARN) Head = "[Warning] ";

		std::cout << Head << Text << std::endl;

		if (LogOutputFile)
		{
			fwrite(Head, strlen(Head), 1, LogOutputFile);
			fwrite(Text, strlen(Text), 1, LogOutputFile);
			fwrite("\r\n", 2, 1, LogOutputFile);
		}
	}
};

static HostInterface G_Host;
static ArcadeLog Log;
static SDL_Window *window = NULL;
static SDL_GLContext glContext;

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

int start(int argc, char *argv[])
{
	LogOutputFile = fopen("output.log", "wb");
	ilInit();

	Runtime::EnableLogging(&Log);
	Runtime::Initialize(&G_Host);


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

	GLuint FrameBufferTexture;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &FrameBufferTexture);

	glBindTexture(GL_TEXTURE_2D, FrameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	PackageManager *PackManager = Runtime::GetPackageManager();
	IHostBuildInterface *Builder = Runtime::GetHostInterface()->GetBuildInterface();

	GamePackage *Package = nullptr;

	IHostFileInterface *FileIO = Runtime::GetHostInterface()->GetFileInterface();
	IFileObject *LuaCode = FileIO->OpenFile("main.lua", FILE_READ);

	Runtime::LogPrint(LOG_MESSAGE, "Game Package: %X", Package);

	if (LuaCode && argc < 2)
	{
		unsigned int SizeOfBuffer = LuaCode->GetSize();
		char *LuaBuffer = (char *)Runtime::Realloc(nullptr, SizeOfBuffer+1);
		LuaCode->Read(LuaBuffer, SizeOfBuffer);
		LuaBuffer[SizeOfBuffer] = 0;

		//std::cout << LuaBuffer << std::endl;

		char PackageName[1024];

		LuaVM VM;
		VM.Init();
		VM.InstallMath();
		VM.InstallCode("main.lua", LuaBuffer, SizeOfBuffer);

		if (!VM.GetGlobal("GAME"))
		{
			Runtime::LogPrint(LOG_ERROR, "'GAME' not specified in main.lua");
		}
		else
		{
			VM.Get(PackageName, 1024);
			Runtime::LogPrint(LOG_MESSAGE, "GamePackage: %s", PackageName);

			Package = PackManager->CreateGamePackage(PackageName);
			Package->Load();
			Package->AddAsset<CodeAsset>("__luamain")->SetCodeBuffer(LuaBuffer, SizeOfBuffer);

			VM.InstallBuild(Package);
			VM.SetFunction("cache");
			VM.Call();

			Package->Save();
		}

		VM.Close();

		IsBuilding = false;

		Runtime::Free(LuaBuffer);
	}
	else
	{
		if (argc < 2)
		{
			Runtime::LogPrint(LOG_ERROR, "main.lua not found in working directory.");
			Runtime::LogPrint(LOG_ERROR, "arcade-tool [PackageName] to run an existing game package.");
			Runtime::Shutdown();
			exit(0);
		}
		else
		{
			IsBuilding = false;

			const char *PackageName = argv[1];
			Package = PackManager->CreateGamePackage(PackageName);
			if (!Package->Load())
			{
				Runtime::Shutdown();
				exit(0);
			}
		}
	}

	if (!Package)
	{
		Runtime::LogPrint(LOG_ERROR, "package not loaded");
		Runtime::Shutdown();
		exit(0);
	}

	Game *MyGame = Runtime::CreateGameInstance(Package);

	bool running = true;
	float lastSwitch = 0;
	float lastTime = 0;
	while (running)
	{
		SDL_PumpEvents();

		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			if (evt.type == SDL_QUIT) {
				running = false;
				break;
			}

			if ((evt.type == SDL_KEYUP || evt.type == SDL_KEYDOWN) && !evt.key.repeat)
			{
				bool Pressed = evt.key.state == SDL_PRESSED;
				const char *KeyName = SDL_GetKeyName(evt.key.keysym.sym);

				MyGame->ControllerEvent(KeyName, Pressed);

				if (evt.key.keysym.scancode == SDL_SCANCODE_R)
				{
					Runtime::DestroyGameInstance(MyGame);
					MyGame = Runtime::CreateGameInstance(Package);
				}
			}
		}

		float time = (float)(SDL_GetTicks()) / 1000.f;
		if (lastTime == 0) lastTime = time;

		MyGame->Update(time - lastTime);

		lastTime = time;

		if (time - lastSwitch > 1.f / 60.f)
		{
			lastSwitch = time;

			//if (MyGame->FrameIsPresentable())
			{
				int GameWidth, GameHeight;
				MyGame->AdvanceFrame();
				MyGame->GetRenderSize(GameWidth, GameHeight);
				glBindTexture(GL_TEXTURE_2D, FrameBufferTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GameWidth, GameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, MyGame->GetFrameBufferRGB24Bit());
			}

			SDL_GL_SwapWindow(window);
		}
		
		glClearColor(0.1f, 0.1f, 0.2f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor4f(1.f, 1.f, 1.f, 1.f);

		int w, h;
		SDL_GetWindowSize(window, &w, &h);

		glViewport(0, 0, w, h);
		glBindTexture(GL_TEXTURE_2D, FrameBufferTexture);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(-1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, -1);
		glEnd();

		Sleep(5);
	}

	Runtime::DestroyGameInstance(MyGame);
	Runtime::Shutdown();

	ilShutDown();

	if (LogOutputFile)
	{
		fclose(LogOutputFile);
	}

	return 0;
}

int SDL_main(int argc, char *argv[])
{
	return start(argc, argv);
}
