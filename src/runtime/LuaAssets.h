/*
* <LuaAssets.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaModule.h"
#include "Package.h"
#include "AssetTypes.h"

#pragma once

class LuaImagePublic : public LuaModule
{
public:
	Arcade::ImageAsset *Ptr;
};

void OpenLuaAssetModule(lua_State *L, Arcade::GamePackage *Package);

Arcade::ImageAsset *lua_toImageAsset(lua_State *L, int idx);
void lua_pushImageAsset(lua_State *L, Arcade::ImageAsset *Image);