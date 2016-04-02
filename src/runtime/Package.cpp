/*
* <Package.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Package.h"
#include "Runtime.h"
#include "AssetTypes.h"
#include "FileInterface.h"
#include "Thread.h"
#include "GLContextHost.h"
#include <map>
#include <vector>
#include <string>
#include "gl/glew.h"

//Current version of the package format
#define GAMEPACKAGE_VERSION 1

using namespace Arcade;

//Map type for correlating strings to IGameAssets
typedef std::map < std::string, IGameAsset * > AssetMapType;

//Vector type listing IGameAssets
typedef std::vector < IGameAsset * > AssetListType;

GamePackage::GamePackage(const char *File, IHostFileInterface *Interface) : FileName(File), FileInterface(Interface), Loaded(false)
{
	//Create asset map and list
	AssetMap = new AssetMapType();
	AssetList = new AssetListType();
}

GamePackage::~GamePackage()
{
	Release();

	//Destroy asset map and list
	delete AssetMap;
	delete AssetList;
}

bool GamePackage::Load(bool HeaderOnly)
{
	//Package is already loaded
	if (Loaded) return true;

//Release any previously loaded data
	Release();

//Open file
	IFileObject *File = FileInterface->OpenFile(FileName, FILE_READ);
	if (!File)
	{
		Runtime::LogPrint(LogMessageType::LOG_ERROR, "Unable to open game package file for reading: %s", FileName);
		FileInterface->CloseFile(File);
		return false;
	}

//Read version
	int VersionNumber; File->Read(&VersionNumber, sizeof(int));
	if (VersionNumber != GAMEPACKAGE_VERSION)
	{
		Runtime::LogPrint(LogMessageType::LOG_ERROR, "Game package file version mismatch: %i != %i", VersionNumber, GAMEPACKAGE_VERSION);
		FileInterface->CloseFile(File);
		return false;
	}

//Get asset count and offset
	int NumAssets; File->Read(&NumAssets, sizeof(int));
	int AssetDataOffset; File->Read(&AssetDataOffset, sizeof(int));

	if (!File->Seek(AssetDataOffset))
	{
		Runtime::LogPrint(LogMessageType::LOG_ERROR, "Game package seek past end of file");
		FileInterface->CloseFile(File);
		return false;
	}

//Load asset headers
	for (int i = 0; i < NumAssets; ++i)
	{
		int Type; File->Read(&Type, sizeof(int));
		int AssetSize; File->Read(&AssetSize, sizeof(int));
		int AssetOffset; File->Read(&AssetOffset, sizeof(int));
		int NameSize; File->Read(&NameSize, sizeof(int));
		char *Name = new char[NameSize + 1]; File->Read(Name, NameSize * sizeof(char));
		Name[NameSize] = 0;

		IGameAsset *Asset = nullptr;

		switch (Type)
		{
		case ASSET_CODE : Asset = AddAsset<CodeAsset>(Name); break;
		case ASSET_FONT : break;
		case ASSET_IMAGE: Asset = AddAsset<ImageAsset>(Name); break;
		case ASSET_SOUND : break;
		default:
			Asset = AddAsset<BlobAsset>(Name); break;
			break;
		}

		if (Asset)
		{
			Asset->SetSize(AssetSize);
			Asset->SetOffset(AssetOffset);
			Asset->SetType((AssetType) Type);
		}

		Runtime::LogPrint(LogMessageType::LOG_MESSAGE, "Asset: %s", Name);

		delete [] Name;
	}

	if (HeaderOnly)
	{
		FileInterface->CloseFile(File);
		return true;
	}

//actually load assets
	AssetMapType *Map = (AssetMapType *)(AssetMap);
	for (auto Pair : *Map)
	{
		IGameAsset *Asset = Pair.second;
		File->Seek(Asset->GetOffset());
		if (!Asset->Load(File))
		{
			Runtime::LogPrint(LogMessageType::LOG_MESSAGE, "Error loading asset: %s (Reason: failed to load)", Pair.first.c_str());
			Asset->SetType(ASSET_NOTLOADED);
			continue;
		}

		if (!Asset->Validate())
		{
			Runtime::LogPrint(LogMessageType::LOG_MESSAGE, "Error loading asset: %s (Reason: corrupted data)", Pair.first.c_str());
			Asset->SetType(ASSET_INVALID);
			continue;
		}

		Runtime::LogPrint(LogMessageType::LOG_MESSAGE, "Loaded Asset: %s [%i bytes]", Pair.first.c_str(), Asset->GetSize());
	}

	Loaded = true;

	FileInterface->CloseFile(File);
	return true;
}

bool GamePackage::Save()
{
//Open file
	IFileObject *File = FileInterface->OpenFile(FileName, FILE_WRITE);
	if (!File)
	{
		Runtime::LogPrint(LogMessageType::LOG_ERROR, "Unable to open game package file for writing: %s", FileName);
		return false;
	}

//Write version
	int VersionNumber = GAMEPACKAGE_VERSION; File->Write(&VersionNumber, sizeof(int));

	Runtime::LogPrint(LogMessageType::LOG_MESSAGE, "Write AssetPack Version: %i", VersionNumber);

//Write asset count and offset
	AssetListType *List = (AssetListType *)(AssetList);
	int NumAssets = (int) List->size(); File->Write(&NumAssets, sizeof(int));
	int AssetOffsetLocation = File->Tell();
	int AssetDataOffset; File->Write(&AssetDataOffset, sizeof(int));

//Write asset data
	AssetMapType *Map = (AssetMapType *)(AssetMap);
	for (auto Pair : *Map)
	{
		IGameAsset *Asset = Pair.second;

		Runtime::LogPrint(LogMessageType::LOG_MESSAGE, "Write Asset: %s", Pair.first.c_str());

		Asset->SetOffset(File->Tell());
		if (!Asset->Save(File))
		{
			Runtime::LogPrint(LOG_ERROR, "Failed to write Asset: %s", Pair.first.c_str());
			Asset->SetSize(0);
		}
		else
		{
			Asset->SetSize(File->Tell() - Asset->GetOffset());
		}
	}

	AssetDataOffset = File->Tell();
	File->Seek(AssetOffsetLocation);
	File->Write(&AssetDataOffset, sizeof(int));
	File->Seek(AssetDataOffset);

//Write asset headers (at end of file)
	for (auto Pair : *Map)
	{
		IGameAsset *Asset = Pair.second;

		int Type = (int) Asset->GetType(); File->Write(&Type, sizeof(int));
		int AssetSize = Asset->GetSize(); File->Write(&AssetSize, sizeof(int));
		int AssetOffset = Asset->GetOffset(); File->Write(&AssetOffset, sizeof(int));
		int NameSize = (int) Pair.first.size(); File->Write(&NameSize, sizeof(int));
		File->Write((void*) Pair.first.c_str(), NameSize);
	}

	FileInterface->CloseFile(File);
	return false;
}

void GamePackage::Release()
{
	AssetMapType *Map = (AssetMapType *)(AssetMap);
	AssetListType *List = (AssetListType *)(AssetList);
	for (IGameAsset *Asset : (*List))
	{
		Asset->Release();
		delete Asset;
	}

	List->clear();
	Map->clear();

	CloseNativeAssets();
}

IGameAsset * Arcade::GamePackage::AddAssetImplementation(IGameAsset *Asset, const char *Name)
{
	AssetMapType *Map = (AssetMapType *)(AssetMap);
	AssetMapType::iterator Iter = Map->find(Name);

	if (Map->find(Name) != Map->end()) RemoveAsset(Name);
	(*Map)[Name] = Asset;

	((AssetListType *)(AssetList))->push_back(Asset);
	return Asset;
}

const char * Arcade::GamePackage::GetFileName()
{
	return FileName;
}

void GamePackage::RemoveAsset(const char *Name)
{
	AssetListType *List = (AssetListType *)(AssetList);
	AssetMapType *Map = (AssetMapType *)(AssetMap);
	AssetMapType::iterator Iter = Map->find(Name);

	if (Iter == Map->end()) return;

	for (unsigned int i = 0; i < List->size(); ++i)
	{
		if ((*List)[i] == (*Iter).second)
		{
			List->erase(List->begin() + i);
			break;
		}
	}

	Map->erase(Iter);
}

int GamePackage::GetNumAssets()
{
	return (int) ((AssetListType *)(AssetList))->size();
}

IGameAsset * GamePackage::GetAssetByID(int Id)
{
	AssetListType *List = (AssetListType *)(AssetList);

	if (Id < 0 || Id > List->size()) return nullptr;
	return (*List)[Id];
}

IGameAsset * GamePackage::GetAssetByName(const char *Name)
{
	AssetMapType *Map = (AssetMapType *)(AssetMap);
	AssetMapType::iterator Iter = Map->find(Name);

	if (Iter != Map->end() )
	{
		return (*Iter).second;
	}

	return nullptr;
}

//GL Stuff
unsigned long AssetUploadGLPtr(void *Ptr)
{
	GLContextHost *Host = Runtime::GetGLContextHost();
	if (!Host || !Host->MakeCurrent())
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to make host context current when loading assets.");
		return 0;
	}

	GamePackage *Package = (GamePackage *)Ptr;
	if (!Package)
	{
		Runtime::LogPrint(LOG_ERROR, "Package ptr was null when loading assets.");
		return 0;
	}

	for (int i = 0; i < Package->GetNumAssets(); ++i)
	{
		IGameAsset *Asset = Package->GetAssetByID(i);
		if (Asset && Asset->GetType() == ASSET_IMAGE)
		{
			ImageAsset *Image = CastAsset<ImageAsset>(Asset);
			if (Image->Handle != HANDLE_UNLOADED) {
				Runtime::LogPrint(LOG_MESSAGE, "IMAGE ALREADY LOADED: %i", Image->Handle);
				continue;
			}

			glGenTextures(1, &Image->Handle);

			glBindTexture(GL_TEXTURE_2D, Image->Handle);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image->GetWidth(), Image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Image->GetPixelsRGBA());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);

			Runtime::LogPrint(LOG_MESSAGE, "LOADED IMAGE TO GL: %i %ix%i", Image->Handle, Image->GetWidth(), Image->GetHeight());
		}
	}

	glFlush();

	return 0;
}

unsigned long AssetCloseGLPtr(void *Ptr)
{
	GLContextHost *Host = Runtime::GetGLContextHost();
	if (!Host || !Host->MakeCurrent())
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to make host context current when closing assets.");
		return 0;
	}

	GamePackage *Package = (GamePackage *)Ptr;
	if (!Package)
	{
		Runtime::LogPrint(LOG_ERROR, "Package ptr was null when loading assets.");
		return 0;
	}

	for (int i = 0; i < Package->GetNumAssets(); ++i)
	{
		IGameAsset *Asset = Package->GetAssetByID(i);
		if (Asset && Asset->GetType() == ASSET_IMAGE)
		{
			ImageAsset *Image = CastAsset<ImageAsset>(Asset);
			if (Image->Handle == HANDLE_UNLOADED) continue;

			glDeleteTextures(1, &Image->Handle);
			Runtime::LogPrint(LOG_MESSAGE, "Unloaded Image %i", Image->Handle);
		}
	}

	glFlush();

	return 0;
}

void GamePackage::UploadNativeAssets(bool ReUpload)
{
	if (ReUpload)
	{
		CloseNativeAssets();
	}

	bool NeedsLoading = false;
	for (int i = 0; i <GetNumAssets(); ++i)
	{
		IGameAsset *Asset = GetAssetByID(i);
		if (Asset && Asset->GetType() == ASSET_IMAGE)
		{
			ImageAsset *Image = CastAsset<ImageAsset>(Asset);
			if (Image->Handle == HANDLE_UNLOADED) { NeedsLoading = true; break; }
		}
	}

	if (NeedsLoading)
	{
		Thread UploadThread(AssetUploadGLPtr, this);
		UploadThread.Resume();
		UploadThread.Wait();
	}
	else
	{
		Runtime::LogPrint(LOG_MESSAGE, "Image assets already uploaded, skipping...");
	}
	Runtime::LogPrint(LOG_MESSAGE, "Images uploaded to OpenGL");
}

void GamePackage::CloseNativeAssets()
{
	Thread CloseThread(AssetCloseGLPtr, this);
	CloseThread.Resume();
	CloseThread.Wait();
}