/*
* <PackageManager.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "PackageManager.h"
#include "Package.h"
#include "FileInterface.h"
#include <map>
#include <string>

using namespace Arcade;

//Map type for correlating strings to GamePackages
typedef std::map<std::string, GamePackage *> PackageMap;

PackageManager::PackageManager(IHostFileInterface *Interface) : FileInterface(Interface)
{
	//Create package map
	Packages = new PackageMap;
}

PackageManager::~PackageManager()
{
	//Delete package map
	delete ((PackageMap *)Packages);
	Packages = nullptr;
}

//Create a game package (or get one if it already exists)
GamePackage * Arcade::PackageManager::CreateGamePackage(const char *Name)
{
	//Cast Packages pointer to a PackageMap so that it can be used
	PackageMap *Map = (PackageMap *) Packages;

	//Find package in map by name
	PackageMap::iterator Found = Map->find(Name);

	//If the package already exists, return it
	if (Found != Map->end()) return (*Found).second;

	//Otherwise create a new package and put it in the map
	GamePackage *Package = new GamePackage(Name, FileInterface);
	(*Map)[Name] = Package;

	//Return the new package
	return Package;
}

//Destroy a game package
void Arcade::PackageManager::DestroyGamePackage(GamePackage *Package)
{
	if (!Package) return;

	//Close native assets and delete the package
	Package->CloseNativeAssets();

	//Cast Packages pointer to a PackageMap so that it can be used
	PackageMap *Map = (PackageMap *)Packages;

	//Find package in map by name
	PackageMap::iterator Found = Map->find(Package->GetFileName());

	//Remove package from map
	if (Found != Map->end())
	{
		Map->erase(Found);
	}

	//Delete package
	delete Package;
}
