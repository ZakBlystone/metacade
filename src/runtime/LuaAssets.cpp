/*
* <LuaAssets.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaAssets.h"
#include "FileInterface.h"
#include "HostInterface.h"
#include "Runtime.h"

using namespace Arcade;

class LuaImage : public LuaImagePublic
{
public:
	static int Size(lua_State *L)
	{
		LuaImage *Img = GET_OBJECT(LuaImage, L, 1);
		if (!Img || !Img->Ptr) return 0;
		
		lua_pushinteger(L, Img->Ptr->GetWidth());
		lua_pushinteger(L, Img->Ptr->GetHeight());
		return 2;
	}

	static int Div(lua_State *L)
	{
		LuaImage *Img = GET_OBJECT(LuaImage, L, 1);
		if (!Img || !Img->Ptr) return 0;

		float x = luaL_checknumber(L, 2);
		float y = luaL_checknumber(L, 3);

		lua_pushnumber(L, x / Img->Ptr->GetWidth());
		lua_pushnumber(L, y / Img->Ptr->GetHeight());

		return 2;
	}

	static int constructor(lua_State *L)
	{
		L = nullptr;
		return 0;
	}

	DEFINE_MODULE(LuaImage, Image)
	MODULE_CONSTRUCTOR(constructor)
	MODULE_FUNCTION(Size)
	MODULE_FUNCTION(Div)
	END_DEFINE_MODULE()
}; CREATE_MODULE(LuaImage)

class AssetModule : public LuaModule
{
public:
	static int Image(lua_State *L);
	static int Lib(lua_State *L);

	DEFINE_MODULE(AssetModule, Asset)
	MODULE_FUNCTION(Image)
	MODULE_FUNCTION(Lib)
	END_DEFINE_MODULE()
}; CREATE_MODULE(AssetModule);

int AssetModule::Image(lua_State *L)
{
	const char *img = luaL_checkstring(L, 1);
	IHostBuildInterface *Builder = Runtime::GetHostInterface()->GetBuildInterface();
	GamePackage *Pack = getUserDataStatic<AssetModule, GamePackage>(L);

	if (Pack)
	{
		IGameAsset *GameAsset = Pack->GetAssetByName(img);
		ImageAsset *Asset = CastAsset<ImageAsset>(GameAsset);

		if (GameAsset && !Asset)
		{
			lua_pushfstring(L, "Asset already exists, but it is not an image: %s", img);
			lua_error(L);
			return 1;
		}

		if (Builder)
		{
			Asset = Pack->AddAsset<ImageAsset>(img);
			if (!Asset)
			{
				lua_pushfstring(L, "Unable create image asset: %s", img);
				lua_error(L);
				return 1;
			}

			if (!Builder->CompileImageAsset(img, Asset))
			{
				Pack->RemoveAsset(img);
				lua_pushfstring(L, "Unable to compile image: %s", img);
				lua_error(L);
				return 1;
			}
		}

		if (Asset)
		{
			//Runtime::LogPrint(LOG_MESSAGE, "Pushing Image Asset");
			PUSH_MODULE(LuaImage, L)->Ptr = Asset;
			return 1;
		}
	}
	else
	{
		lua_pushfstring(L, "Package not loaded");
		lua_error(L);
		return 1;
	}

	return 0;
}

int AssetModule::Lib(lua_State *L)
{
	const char *lib = luaL_checkstring(L, 1);
	IHostBuildInterface *Builder = Runtime::GetHostInterface()->GetBuildInterface();
	GamePackage *Pack = getUserDataStatic<AssetModule, GamePackage>(L);

	if (Pack)
	{
		IGameAsset *GameAsset = Pack->GetAssetByName(lib);
		CodeAsset *Asset = CastAsset<CodeAsset>(GameAsset);

		if (Builder)
		{
			IHostFileInterface *File = Runtime::GetHostInterface()->GetFileInterface();
			IFileObject *CodeFile = File->OpenFile(lib, FILE_READ);
			if (CodeFile)
			{
				unsigned int size = CodeFile->GetSize();
				char *buffer = (char*) Runtime::Realloc(nullptr, size+1);

				if (CodeFile->Read(buffer, size))
				{
					Asset = Pack->AddAsset<CodeAsset>(lib);

					buffer[size] = 0;
					Asset->SetCodeBuffer(buffer, size+1);
					if (!Asset->Validate())
					{
						Runtime::LogPrint(LOG_ERROR, "Include file contains invalid characters: %s", lib);
					}
				}
				else
				{
					Runtime::LogPrint(LOG_ERROR, "Error reading include file: %s", lib);
				}

				Runtime::Free(buffer);
			}
			else
			{
				Runtime::LogPrint(LOG_ERROR, "Unable to include: %s", lib);
			}
		}

		if (Asset && Asset->Validate())
		{
			const char *Buffer = Asset->GetCodeBuffer();
			if (!Buffer) return 0;

			unsigned int Size = Asset->GetSize();

			luaL_dostring(L, Buffer);

			/*if (luaL_loadbuffer(L, Buffer, Size, lib) || lua_pcall(L, 0, 0, 0)) {
				Runtime::LogPrint(LOG_ERROR, "Lua: [%s] %s", lib, lua_tostring(L, -1));
				lua_pop(L, 1);
			}*/

			Runtime::LogPrint(LOG_MESSAGE, "Include Lib: %s", lib);
			//Asset
		}
	}
	return 0;
}

void OpenLuaAssetModule(lua_State *L, GamePackage *Package)
{
	OPEN_MODULE_USERDATA(L, AssetModule, Package);
	OPEN_MODULE(L, LuaImage);
}

Arcade::ImageAsset * lua_toImageAsset(lua_State *L, int idx)
{
	LuaImage *Img = GET_OBJECT(LuaImage, L, idx);
	if (!Img || !Img->Ptr) return nullptr;
	return Img->Ptr;
}

void lua_pushImageAsset(lua_State *L, Arcade::ImageAsset *Image)
{
	PUSH_MODULE(LuaImage, L)->Ptr = Image;
}
