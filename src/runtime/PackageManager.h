/*
* <PackageManager.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Runtime.h"

#pragma once

namespace Arcade
{
	//
	//PackageManager is the frontend for loading GamePackages from a filesystem
	//
	class IHostFileInterface;
	class GamePackage;
	class RUNTIME_API PackageManager
	{
	public:
		//Not implemented yet
		void AddContentDirectory(const char *Dir);

		//Not implemented yet
		void LoadAllGamePackages();

		//Create a game package (or get one if it already exists)
		GamePackage *CreateGamePackage(const char *Name);

		//Destroy a game package
		void DestroyGamePackage(GamePackage *Package);

	private:
		//These methods are only exposed to the PackageManager and the Runtime class
		friend class Runtime;
		
		//Private constructor and destructor, only the Runtime may create / destroy PackageManager instances
		PackageManager(IHostFileInterface *Interface);
		~PackageManager();

		//File interface
		IHostFileInterface *FileInterface;

		//Internal type pointer
		void *Packages;
	};

}