/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
lua_vm.cpp:
===============================================================================
*/

#include "lua_private.h"

#include <iostream>
#include <fstream>

static void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
	unsigned int *Usage = (unsigned int *)ud;

	if (nsize == 0) {
		*Usage -= (unsigned int)osize;
		free(ptr);
		return NULL;
	}
	else
	{
		*Usage += (unsigned int)(nsize - osize);
		return realloc(ptr, nsize);
	}
}

static int l_print(lua_State *S)
{
	const char *STR = lua_tostring(S, 1);

	std::cout << STR << std::endl;
	return 0;
}


LuaVM::LuaVM()
	: _L(nullptr)
	, _memUsage(0)
{

}

LuaVM::~LuaVM()
{
	shutdown();
}

Arcade::ELanguage LuaVM::getLanguage()
{
	return LANG_LUA;
}

bool LuaVM::init()
{
	if ( _L != nullptr ) return true;

	_memUsage = 0;
	_L = lua_newstate(l_alloc, &_memUsage);

	luaJIT_setmode(_L, -1, LUAJIT_MODE_DEBUG | LUAJIT_MODE_ON);

	luaopen_base(_L);
	luaopen_bit(_L);
	luaopen_math(_L);
	luaopen_table(_L);

	OpenLuaMathModule(_L);

	//check errors

	return true;
}

void LuaVM::shutdown()
{
	if ( _L == nullptr ) return;

	lua_close(_L);

	_L = nullptr;
}

bool Arcade::LuaVM::isRunning()
{
	return _L != nullptr;
}

Arcade::IVMClass* LuaVM::loadGameVMClass()
{
	std::fstream input("test.lua", std::ios::binary | std::ios::in | std::ios::ate);

	if ( input.is_open() )
	{
		uint32 size = input.tellg();
		input.seekg(0);

		if ( size == 0 ) return nullptr;
		
		char *buffer = new char[size];
		input.read(buffer, size);

		if (luaL_loadbuffer(_L, buffer, size, "main") || lua_pcall(_L, 0, 0, 0)) {
			std::cout << "Lua: main: " << lua_tostring(_L, -1);
			lua_pop(_L, 1);
			return nullptr;
		}
	}

	return nullptr;
}

bool LuaVM::includeGameScript()
{
	return false;
}

bool LuaVM::validateGameScript()
{
	return false;
}

extern shared_ptr<IVMHost> Arcade::getLuaVM()
{
	return make_shared<LuaVM>();
}
