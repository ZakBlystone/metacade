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
	, _metaData(make_shared<CMetaData>())
	, CRuntimeObject(host.get())
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

shared_ptr<CMetaData> Arcade::CLuaVMClass::getMetaData()
{
	return _metaData;
}

class IVMHost* Arcade::CLuaVMClass::getHost()
{
	return _host.get();
}

int CLuaVMClass::metaFunctionCreate(lua_State *L)
{
	lua_getfield(L, 1, "__klass");

	CLuaVMClass *klass = (CLuaVMClass *) lua_touserdata(L, -1);
	const char *key = lua_tostring(L, 2);

	if ( klass == nullptr ) return 0;

	int type = lua_type(L, 3);
	if ( type == LUA_TFUNCTION )
	{
		lua_getglobal(L, "_G");
		lua_setfenv(L, 3);

		/*if(luaJIT_setmode(L, 3, LUAJIT_MODE_ALLSUBFUNC | LUAJIT_MODE_ON))
		{
			klass->log(LOG_MESSAGE, "JIT Enable for function: %s", key);
		}
		else
		{
			klass->log(LOG_ERROR, "FAILED to JIT Enable for function: %s", key);
		}*/

		auto entry = make_pair(std::string(key), make_shared<LuaVMReference>(klass->_host, 3));
		klass->_functions.insert(entry);
	}
	else
	{
		luaL_error(L, "functions only");
	}

	return 0;
}

int CLuaVMClass::metaTextureCreate(lua_State *L)
{
	CLuaVMClass* klass = (CLuaVMClass*) lua_touserdata(L, lua_upvalueindex(1));
	const char* key = lua_tostring(L, 2);

	if ( klass == nullptr ) return 0;
	int type = lua_type(L, 3);

	if ( type == LUA_TSTRING )
	{
		klass->_textureLoadArgs.insert(make_pair(CString(key), CString(lua_tostring(L, 3))));
	}

	return 0;
}

int CLuaVMClass::metaKeyvalCreate(lua_State *L)
{
	CLuaVMClass* klass = (CLuaVMClass*) lua_touserdata(L, lua_upvalueindex(1));
	const char* key = lua_tostring(L, 2);

	if ( klass == nullptr ) return 0;
	int type = lua_type(L, 3);

	if ( type == LUA_TSTRING )
	{
		klass->_metaData->setKeyValuePair(key, lua_tostring(L, 3));
	}
	return 0;
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

void CLuaVMClass::createMetaTable(const char* name, lua_CFunction target)
{
	lua_State *L = _host->getState();

	//TABLE
	lua_newtable(L);

		//METATABLE
		lua_newtable(L);
		lua_pushlightuserdata(L, this);
		lua_pushcclosure(L, target, 1); //could pass values here
		lua_setfield(L, -2, "__newindex");
		//DONE METATABLE

	lua_setmetatable(L, -2);
	lua_setfield(L, -2, name);
	//DONE TABLE
}

bool CLuaVMClass::loadFromPackage(shared_ptr<CPackage> package)
{
	lua_State *L = _host->getState();
	_functions.clear();

	CCodeAsset* luaMain = castAsset<CCodeAsset>( package->findAssetByName("main.lua") );
	if ( luaMain == nullptr )
	{
		log(LOG_ERROR, "Failed to load 'main.lua'");
		return false;
	}

	if (luaL_loadbuffer(L, luaMain->getCodeBuffer(), luaMain->getCodeLength(), *luaMain->getName()))
	{
		std::cout << "Lua: " << *luaMain->getName() << " : " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
		return false;	
	}

	lua_newtable(L);

	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "__klass");

	lua_newtable(L);
	createMetaTable("textures", CLuaVMClass::metaTextureCreate);
	createMetaTable("info", CLuaVMClass::metaKeyvalCreate);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_pushcclosure(L, CLuaVMClass::metaFunctionCreate, 0);
	lua_setfield(L, -2, "__newindex");

	lua_setmetatable(L, -2);
	lua_setfenv(L, -2);

	if (lua_pcall(L, 0, 0, 0)) {
		std::cout << "Lua: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
		return false;
	}

	return true;
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

		if (luaL_loadbuffer(L, buffer, size, filename.c_str()))
		{
			std::cout << "Lua: " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
			return false;	
		}

		lua_newtable(L);

		lua_pushlightuserdata(L, this);
		lua_setfield(L, -2, "__klass");

		lua_newtable(L);
		createMetaTable("textures", CLuaVMClass::metaTextureCreate);
		createMetaTable("info", CLuaVMClass::metaKeyvalCreate);

		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		lua_pushcclosure(L, CLuaVMClass::metaFunctionCreate, 0);
		lua_setfield(L, -2, "__newindex");

		lua_setmetatable(L, -2);
		lua_setfenv(L, -2);

		if (lua_pcall(L, 0, 0, 0)) {
			std::cout << "Lua: " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
			return false;
		}

		return true;
	}

	return false;
}