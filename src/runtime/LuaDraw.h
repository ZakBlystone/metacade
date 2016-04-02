/*
* <LuaDraw.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaModule.h"
#include "Game.h"

#pragma once

void OpenLuaDrawModule(lua_State *L, class Arcade::Game *GameInstance);