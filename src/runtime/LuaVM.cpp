/*
* <LuaVM.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Package.h"
#include "Game.h"
#include "LuaVM.h"
#include "AssetTypes.h"
#include "LuaDraw.h"
#include "LuaAssets.h"
#include "LuaMath.h"
#include "LuaInput.h"
#include "lua.hpp"

using namespace Arcade;

#define L ((lua_State *)(State))

static int l_print(lua_State *S)
{
	const char *STR = lua_tostring(S, 1);

	Runtime::LogPrint(LOG_MESSAGE, STR ? STR : "nil");
	return 0;
}

static void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
	unsigned int *Usage = (unsigned int *)ud;

	if (nsize == 0) {
		*Usage -= (unsigned int)osize;
		Runtime::Free(ptr);
		return NULL;
	}
	else
	{
		*Usage += (unsigned int)(nsize - osize);
		return Runtime::Realloc(ptr, nsize);
	}
}

unsigned int LuaVM::GetMemUsage()
{
	return MemUsage;
}

bool LuaVM::Init()
{
	MemUsage = 0;

	Runtime::LogPrint(LOG_MESSAGE, " -Lua state");
	State = lua_newstate(l_alloc, &this->MemUsage);

	if (!State)
	{
		Runtime::LogPrint(LOG_WARN, "Unable to create Lua state with memory allocation, falling back to standard allocation mode.");
		State = lua_open();
		if (!State)
		{
			Runtime::LogPrint(LOG_ERROR, "LuaVM Creation Failed!");
			return false;
		}
	}

	Runtime::LogPrint(LOG_MESSAGE, " -Lua jit mode");
	luaJIT_setmode(L, -1, LUAJIT_MODE_DEBUG | LUAJIT_MODE_ON);

	Runtime::LogPrint(LOG_MESSAGE, " -Lua libs");
	//luaL_openlibs(L);
	luaopen_base(L);
	luaopen_bit(L);
	luaopen_math(L);
	luaopen_table(L);
	lua_pushcfunction(L, l_print);
	lua_setglobal(L, "print");
	lua_pop(L, 1);

	Runtime::LogPrint(LOG_MESSAGE, " -Done");

	return true;
	//lua_setallocf(L, l_alloc, this);
}

void LuaVM::Close()
{
	lua_close(L);
	State = NULL;
}

bool LuaVM::InstallCode(const char *Name, const char *Buffer, unsigned int Size)
{
	if (luaL_loadbuffer(L, Buffer, Size, Name) || lua_pcall(L, 0, 0, 0)) {
		Runtime::LogPrint(LOG_ERROR, "Lua: [%s] %s", Name, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}
	return true;
}

bool LuaVM::InstallCode(const char *Name, const char *Buffer)
{
	return InstallCode(Name, Buffer, (unsigned int) strlen(Buffer));
}

bool LuaVM::InstallCode(Arcade::CodeAsset *Code)
{
	if (!Code) return false;
	return InstallCode("CODE", Code->GetCodeBuffer(), Code->GetSize());
}

void LuaVM::InstallRuntime(Arcade::Game *GameInstance)
{
	OpenLuaInputModule(L, GameInstance);
	OpenLuaAssetModule(L, GameInstance->GetPackage());
	OpenLuaDrawModule(L, GameInstance);
}

void LuaVM::InstallBuild(Arcade::GamePackage *BuildTarget)
{
	OpenLuaAssetModule(L, BuildTarget);
}

bool LuaVM::SetFunction(const char *Function)
{
	CallArgs = 0;
	lua_getglobal(L, Function);
	if (lua_type(L, -1) != LUA_TFUNCTION) {
		Runtime::LogPrint(LOG_ERROR, "Couldn't find function: %s", Function);
		return false;
	}

	return true;
}

bool LuaVM::Call(int ReturnArgs)
{
	if (lua_pcall(L, CallArgs, ReturnArgs, 0) != 0)
	{
		Runtime::LogPrint(LOG_ERROR, lua_tostring(L, -1));
		CallArgs = 0;
		return false;
	}
	else
	{
		//lua_pop(L, 1);
		CallArgs = 0;
		return true;
	}
}


void LuaVM::Push(int Value)
{
	lua_pushinteger(L, Value);
	++CallArgs;
}

void LuaVM::Push(float Value)
{
	lua_pushnumber(L, Value);
	++CallArgs;
}

void LuaVM::Push(const char *Value)
{
	lua_pushstring(L, Value);
	++CallArgs;
}

bool LuaVM::Get(int &Value)
{
	if (lua_type(L, -1) != LUA_TNUMBER) return false;

	Value = lua_tointeger(L, -1);

	return true;
}

bool LuaVM::Get(float &Value)
{
	if (lua_type(L, -1) != LUA_TNUMBER) return false;

	Value = lua_tonumber(L, -1);

	return true;
}

bool LuaVM::Get(char *Value, unsigned int BufferSize)
{
	if (lua_type(L, -1) != LUA_TSTRING) return false;

	const char *str = lua_tostring(L, -1);
	if (!str) return false;

	size_t len = strlen(str);
	memcpy(Value, str, len<BufferSize ? len : BufferSize);

	if (len < BufferSize - 1)
	{
		Value[len] = 0;
	}

	return true;
}

bool LuaVM::GetGlobal(const char *Name)
{
	lua_getglobal(L, Name);
	return lua_type(L, -1) != LUA_TNIL;
}

void LuaVM::InstallMath()
{
	OpenLuaMathModule(L);
}
