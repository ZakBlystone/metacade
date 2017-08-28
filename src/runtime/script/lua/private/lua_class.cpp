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

Arcade::CLuaVMClass::CLuaVMClass(weak_ptr<CLuaVM> host)
	: _host(host)
	, _metaData(make_shared<CMetaData>())
	, CRuntimeObject(host.lock().get())
{

}

Arcade::CLuaVMClass::~CLuaVMClass()
{
	log(LOG_MESSAGE, "Destruct luaVMClass");
}

bool Arcade::CLuaVMClass::reload()
{
	return false;
}

shared_ptr<CMetaData> Arcade::CLuaVMClass::getMetaData()
{
	return _metaData;
}

class IVMHost* Arcade::CLuaVMClass::getHost()
{
	return _host.lock().get();
}

int CLuaVMClass::metaTopLevelSet(lua_State *L)
{
	lua_getfield(L, 1, "__klass");

	CLuaVMClass *klass = (CLuaVMClass *) lua_touserdata(L, -1);
	const char *key = lua_tostring(L, 2);

	if ( klass == nullptr ) return 0;

	int type = lua_type(L, 3);
	if ( type == LUA_TFUNCTION )
	{
		auto entry = make_pair(CString(key), make_shared<LuaVMReference>(klass->getLuaHost(), 3));
		klass->_functions.insert(entry);
	}
	else
	{
		lua_getfield(L, 1, "__locals");
		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
		lua_rawset(L, -3);
	}

	lua_pop(L, 1);

	return 0;
}

int Arcade::CLuaVMClass::metaTopLevelGet(lua_State *L)
{
	lua_getfield(L, 1, "__locals");
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	lua_remove(L, -3);

	return 1;
}

shared_ptr<IVMInstance> Arcade::CLuaVMClass::createVMInstance()
{
	return shared_ptr<CLuaVMInstance>( new CLuaVMInstance(shared_from_this()) );
}

bool Arcade::CLuaVMClass::pushLuaFunction(CString functionName) const
{
	auto found = _functions.find(functionName);
	if ( found != _functions.end() )
	{
		(*found).second->push();
		return true;
	}

	return false;
}

void Arcade::CLuaVMClass::pushLocalTable() const
{
	_locals->push();
}

bool CLuaVMClass::loadFromPackage(weak_ptr<CPackage> package)
{
	_package = package;

	shared_ptr<CPackage> locked = package.lock();
	if ( locked == nullptr ) return false;

	shared_ptr<CLuaVM> host = getLuaHost().lock();

	lua_State *L = host->getState();
	_functions.clear();

	CCodeAsset* luaMain = castAsset<CCodeAsset>( locked->findAssetByName("main.lua") );
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
	_locals = make_shared<LuaVMReference>(getLuaHost(), -1);
	lua_setfield(L, -2, "__locals");

	lua_newtable(L);
	lua_pushcclosure(L, CLuaVMClass::metaTopLevelGet, 0);
	lua_setfield(L, -2, "__index");

	lua_pushcclosure(L, CLuaVMClass::metaTopLevelSet, 0);
	lua_setfield(L, -2, "__newindex");

	lua_setmetatable(L, -2);
	lua_setfenv(L, -2);

	if (lua_pcall(L, 0, 0, 0)) {
		std::cout << "Lua: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
		return false;
	}

	for ( int32 i=0; i<locked->getNumAssets(); ++i )
	{
		CCodeAsset* lib = castAsset<CCodeAsset>( locked->getAsset(i) );
		if ( lib && lib != luaMain )
		{
		
			if (luaL_loadbuffer(L, lib->getCodeBuffer(), lib->getCodeLength(), *lib->getName()))
			{
				std::cout << "Lua: " << *lib->getName() << " : " << lua_tostring(L, -1) << std::endl;
				lua_pop(L, 1);
			}
			else
			{
			
				if (lua_pcall(L, 0, 0, 0)) {
					std::cout << "Lua: " << lua_tostring(L, -1) << std::endl;
					lua_pop(L, 1);
				}

			}

		}
	}

	return true;
}

CPackage* CLuaVMClass::getPackage() const
{
	return _package.lock().get();
}
