/*
* <Runtime.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Logging.h"
#include "HostInterface.h"

#pragma once

#ifdef WINDOWS
#if defined(RUNTIME_API_EXPORT)
#   define RUNTIME_API   __declspec(dllexport)
#else // outside DLL
#   define RUNTIME_API   __declspec(dllimport)
#endif
#endif

namespace Arcade
{
	class IAllocator
	{
	public:
		virtual void *Realloc(void *Ptr, unsigned int NewSize) = 0;
		virtual void Free(void *Ptr) = 0;
	};

	class GLContextHost;
	class PackageManager;
	class GamePackage;
	class Game;
	class RUNTIME_API Runtime
	{
	public:
		static void EnableLogging(IArcadeLogCallback *Callback);
		static bool Initialize(IHostInterface *Host, IAllocator *CustomAllocator = nullptr);
		static void Shutdown();

		static GLContextHost *GetGLContextHost();
		static PackageManager *GetPackageManager();
		static IHostInterface *GetHostInterface();
		static Game *CreateGameInstance(GamePackage *Package);
		static void DestroyGameInstance(Game *GameInstance);

		static void LogPrint(LogMessageType Type, const char *Message, ...);
		static void *Realloc(void *Ptr, unsigned int NewSize);
		static void Free(void *Ptr);

	private:
		static bool Initialized;
		static IHostInterface *HostInterface;
		static IArcadeLogCallback *LogCallback;
		static IAllocator *Allocator;
		static PackageManager *PackManager;
		static GLContextHost *RenderContextHost;
	};

}