/*
* <Package.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Runtime.h"

#pragma once

namespace Arcade
{
	//
	//GamePackage contains a list of IGameAssets
	//
	class IHostFileInterface;
	class IHostInterface;
	class RUNTIME_API GamePackage
	{
	public:
		//Load assets from package (calling with HeaderOnly set to true only loads header and not the assets themselves)
		bool Load(bool HeaderOnly = false);

		//Save assets out to disk
		bool Save();

		//Release assets from memory
		void Release();

		//Templatized method for adding an asset where 'T' is the class of asset to add (i.e. 'ImageAsset')
		template<typename T>
		T *AddAsset(const char *Name)
		{
			T *Asset = new T;
			Asset = (T *) AddAssetImplementation(Asset, Name);
			if (Asset == nullptr)
			{
				delete Asset;
				return nullptr;
			}
			return Asset;
		}

		//Remove asset from the package by name
		void RemoveAsset(const char *Name);

		//Get number of assets in this package
		int GetNumAssets();

		//Get asset by index
		class IGameAsset *GetAssetByID(int Id);

		//Get asset by name
		class IGameAsset *GetAssetByName(const char *Name);

		//Upload assets to the runtime
		void UploadNativeAssets(bool ReUpload = false);

		//Unload assets from the runtime
		void CloseNativeAssets();

		//Get name of this package
		const char *GetFileName();

	private:
		//These methods are only exposed to the Package and the PackageManager class
		friend class PackageManager;

		//Internal implementation for adding an asset
		class IGameAsset *AddAssetImplementation(class IGameAsset *Asset, const char *Name);

		//Private constructor and destructor, only the PackageManager may create / destroy Package instances
		GamePackage(const char *File, IHostFileInterface *Interface);
		~GamePackage();

		const char *FileName;
		IHostFileInterface *FileInterface;

		void *AssetMap, *AssetList;
		bool Loaded;
	};

}