/*
* <LuaMath.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaModule.h"
#include "Game.h"

#pragma once

bool lua_toVec2(lua_State *L, int idx, float &x, float &y);
void lua_pushVec2(lua_State *L, float x, float y);
float *lua_toMatrix3x3(lua_State *L, int idx);
void lua_pushMatrix3x3(lua_State *L, float *Mtx);

void OpenLuaMathModule(lua_State *L);