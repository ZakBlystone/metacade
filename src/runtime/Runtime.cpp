/*
* <Runtime.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Runtime.h"
#include "Game.h"
#include "HostInterface.h"
#include "FileInterface.h"
#include "PackageManager.h"
#include "GLContextHost.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>

using namespace Arcade;

typedef std::list<Game*> GameInstanceList;

IHostInterface *Runtime::HostInterface = nullptr;
IArcadeLogCallback *Runtime::LogCallback = nullptr;
IAllocator *Runtime::Allocator = nullptr;
PackageManager *Runtime::PackManager = nullptr;
GLContextHost *Runtime::RenderContextHost = nullptr;
bool Runtime::Initialized = false;

static GameInstanceList GameInstances;

void Runtime::EnableLogging(IArcadeLogCallback *Callback)
{
	LogCallback = Callback;
}

void Runtime::LogPrint(LogMessageType Type, const char *Message, ...)
{
	if (LogCallback)
	{
		va_list args;
		int len;
		char *buffer;

		va_start(args, Message);
		len = _vscprintf(Message, args) + 1;

		buffer = new char[len];
		vsprintf_s(buffer, len, Message, args);

		LogCallback->LogMessage(buffer, Type);

		delete[] buffer;

		va_end(args);
	}
}

bool Runtime::Initialize(IHostInterface *Host, IAllocator *CustomAllocator)
{
	if (!Host) return false;
	if (Initialized) Shutdown();

	HostInterface = Host;
	Allocator = CustomAllocator;
	PackManager = new PackageManager(Host->GetFileInterface());
	RenderContextHost = new GLContextHost();
	
	RenderContextHost->Init();

	if (!PackManager) return false;

	Initialized = true;
	LogPrint(LOG_MESSAGE, "Initialized Arcade System");
	return true;
}

void Runtime::Shutdown()
{
	if (!Initialized) return;

	for (Game *Instance : GameInstances)
	{
		delete Instance;
	}
	GameInstances.clear();

	LogPrint(LOG_MESSAGE, "Shutdown Arcade System");

	RenderContextHost->Shutdown();

	delete RenderContextHost;
	delete PackManager;
	PackManager = nullptr;
	Allocator = nullptr;
	HostInterface = nullptr;
	RenderContextHost = nullptr;

	Initialized = false;
}

PackageManager * Runtime::GetPackageManager()
{
	return PackManager;
}

void * Runtime::Realloc(void *Ptr, unsigned int NewSize)
{
	if (Allocator) return Allocator->Realloc(Ptr, NewSize);
	return realloc(Ptr, NewSize);
}

void Runtime::Free(void *Ptr)
{
	if (Allocator) return Allocator->Free(Ptr);
	return free(Ptr);
}

IHostInterface * Runtime::GetHostInterface()
{
	return HostInterface;
}

Game * Runtime::CreateGameInstance(GamePackage *Package)
{
	Game *Instance = new Game(RenderContextHost, Package);
	GameInstances.push_back(Instance);
	return Instance;
}

void Runtime::DestroyGameInstance(Game *GameInstance)
{
	GameInstances.remove(GameInstance);
	delete GameInstance;
}

GLContextHost * Arcade::Runtime::GetGLContextHost()
{
	return RenderContextHost;
}
