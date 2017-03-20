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
lua_class.cpp:
===============================================================================
*/

#include "lua_private.h"

#include <fstream>

Arcade::CLuaVMClass::CLuaVMClass(shared_ptr<CLuaVM> host)
	: _host(host)
{

}

Arcade::CLuaVMClass::~CLuaVMClass()
{

}

bool Arcade::CLuaVMClass::reload()
{
	if ( _lastLoadFile != "" )
	{
		return loadFromFile(_lastLoadFile);
	}
	return false;
}

class CMetaData* Arcade::CLuaVMClass::getMetaData()
{
	return nullptr;
}

class IVMHost* Arcade::CLuaVMClass::getHost()
{
	return _host.get();
}

shared_ptr<IVMInstance> Arcade::CLuaVMClass::createVMInstance()
{
	return shared_ptr<CLuaVMInstance>( new CLuaVMInstance(shared_from_this()) );
}

bool Arcade::CLuaVMClass::pushLuaFunction(string functionName) const
{
	auto found = _functions.find(functionName);
	if ( found != _functions.end() )
	{
		(*found).second->push();
		return true;
	}
	return false;
}

bool Arcade::CLuaVMClass::loadFromFile(string filename)
{
	_lastLoadFile = filename;
	_functions.clear();

	std::fstream input(filename, std::ios::binary | std::ios::in | std::ios::ate);

	lua_State *L = _host->getState();

	if ( input.is_open() )
	{
		uint32 size = (uint32) input.tellg();
		input.seekg(0);

		if ( size == 0 ) return false;
		
		char *buffer = new char[size];
		input.read(buffer, size);

		if (luaL_loadbuffer(L, buffer, size, "main"))
		{
			std::cout << "Lua: main: " << lua_tostring(L, -1);
			lua_pop(L, 1);
			return nullptr;	
		}

		lua_newtable(L);

		lua_pushlightuserdata(L, this);
		lua_setfield(L, -2, "__klass");

		lua_newtable(L);
		lua_newtable(L);
		lua_setfield(L, -2, "sounds");

		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		lua_pushcclosure(L, CLuaVMClass::testMetaSet, 0);
		lua_setfield(L, -2, "__newindex");

		lua_setmetatable(L, -2);
		lua_setfenv(L, -2);

		if (lua_pcall(L, 0, 0, 0)) {
			std::cout << "Lua: main: " << lua_tostring(L, -1);
			lua_pop(L, 1);
			return false;
		}

		return true;
	}

	return false;
}