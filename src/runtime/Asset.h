/*
* <Asset.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#pragma once

namespace Arcade
{
	//Magic number for an asset that doesn't contain a handle to a natively loaded instance (such as a GL texture)
	const unsigned int HANDLE_UNLOADED = 0;

	//
	//Enum for types of assets:
	//
	enum AssetType
	{
		//Special cases
		ASSET_NOTLOADED,	//Asset hasn't been loaded from a package yet.
		ASSET_INVALID,		//Asset was loaded but contains corrupt or invalid data.

		//Asset classes, when adding a new asset type, add an enum for it here
		ASSET_CODE,
		ASSET_IMAGE,
		ASSET_FONT,
		ASSET_SOUND,
		ASSET_BLOB,
	};

	//
	//Asset interface (IGameAsset) all asset types inherit this.
	//
	class IFileObject;
	class IGameAsset
	{
	public:
		virtual bool Load(IFileObject *Object) = 0;		//Callback for loading Asset data from package
		virtual bool Save(IFileObject *Object) = 0;		//Callback for saving Asset data to package
		virtual bool Validate() = 0;					//Check to make sure Asset data is valid (security / stability)
		virtual void Release() = 0;						//Release Asset data (free from memory)
		virtual int GetSize() = 0;						//Size of asset in package
		virtual AssetType GetType() = 0;				//Get corresponding Enum for this asset

	private:

		//These methods are only exposed to the Asset and the Package class
		friend class GamePackage;

		virtual void SetSize(int Size) = 0;				//The size of the Asset in the package
		virtual void SetOffset(int Offset) = 0;			//Set the offset of the Asset in the package format
		virtual int GetOffset() = 0;					//Get the offset of the Asset in the package format

		virtual void SetType(AssetType Type) = 0;		//Set the type of this asset so that it can be casted properly
	};

	//
	//Asset type template (GameAsset<T>) where 'T' is the asset type Enum this asset should use.
	//
	template <AssetType T>
	class GameAsset : public IGameAsset
	{
	public:
		//Set defaults for variables in the Asset constructor
		GameAsset() : Type(T), Size(0), Handle(HANDLE_UNLOADED) {}

		//Implementation for interface functions, as GameAsset contains the concrete data
		bool CheckType() { return Type == T; }
		virtual int GetSize() { return Size; }
		virtual AssetType GetType() { return Type; }

		//Handle to native object
		unsigned int Handle;

	private:
		//Implementation for interface functions, as GameAsset contains the concrete data
		virtual void SetSize(int Size) { this->Size = Size; }
		virtual void SetOffset(int Offset) { this->Offset = Offset; }
		virtual int GetOffset() { return this->Offset;  }

		virtual void SetType(AssetType Type) { this->Type = Type; }

		AssetType Type;		//Asset type
		int Size;			//Asset size
		int Offset;			//Asset offset
	};

	//Cast an IGameAsset instance to Asset class T, returns null if the asset is not of type T or if it is null
	template<typename T>
	T* CastAsset(IGameAsset *Asset) { if (!Asset || !((T*)(Asset))->CheckType()) return nullptr; return (T*)Asset; }
}