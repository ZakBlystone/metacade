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

CLuaVM::CLuaVM(CRuntimeObject* outer)
	: _L(nullptr)
	, _memUsage(0)
	, CRuntimeObject(outer)
{

}

CLuaVM::~CLuaVM()
{
	log(LOG_MESSAGE, "Destruct luaVM");
	shutdown();
}

Arcade::ELanguage CLuaVM::getLanguage()
{
	return LANG_LUA;
}

bool CLuaVM::init()
{
	if ( _L != nullptr ) return true;

	_memUsage = 0;
	_L = luaL_newstate(); //lua_newstate(l_alloc, &_memUsage);

	luaopen_jit(_L);

	if (luaJIT_setmode(_L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON) )
	{
		log(LOG_MESSAGE, "LuaJIT ready");
	}
	else
	{
		log(LOG_ERROR, "Failed to enable LuaJIT");
	}

	//luaJIT_setmode(_L, -1, LUAJIT_MODE_DEBUG | LUAJIT_MODE_ON);
	//luaJIT_setmode(_L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);

	luaopen_base(_L);
	luaopen_bit(_L);
	luaopen_math(_L);
	luaopen_table(_L);
	luaopen_string(_L);

	lua_pushlightuserdata(_L, getRuntime());
	lua_setglobal(_L, "__runtime");

	lua_newtable(_L);

	for ( int32 i=0; i<EHIDKeyCode::KEY_MAX; ++i )
	{
		lua_pushinteger(_L, i);
		lua_setfield(_L, -2, *CHIDKeyCodes::getKeyName( (EHIDKeyCode)i ));
	}

	lua_setglobal(_L, "_keys");

	lua_newtable(_L);

	for ( int32 i=0; i<EHIDKeyCode::KEY_MAX; ++i )
	{
		lua_pushinteger(_L, i);
		lua_pushstring(_L, *CHIDKeyCodes::getKeyName( (EHIDKeyCode)i ));
		lua_rawset(_L, -3);
	}

	lua_setglobal(_L, "_keynames");

	//OpenLuaMathModule(_L);
	OpenLuaDrawModule(_L);
	openAssetModule(_L);
	openLuaSoundModule(_L);

	//check errors

	return true;
}

void CLuaVM::shutdown()
{
	if ( _L == nullptr ) return;

	lua_close(_L);

	_L = nullptr;
}

bool Arcade::CLuaVM::isRunning()
{
	return _L != nullptr;
}

bool Arcade::CLuaVM::pcall(int nargs)
{
	if (lua_pcall(_L, nargs, 0, 0)) 
	{
		log(LOG_WARN, "Lua Error: %s", lua_tostring(_L, -1));
		lua_pop(_L, 1);
		//exit(0);
		return false;
	}
	return true;
}

lua_State* Arcade::CLuaVM::getState()
{
	return _L;
}

bool CLuaVM::pushVariant(const CVariant& variant)
{
	switch(variant.type())
	{
	case Arcade::VT_NONE:
		lua_pushnil(_L);
		return true;
	break;
	case Arcade::VT_BOOLEAN:
		{
			bool b;
			if ( variant.get(b) ) { lua_pushboolean(_L, b); return true; }
		}
	break;
	case Arcade::VT_UINT:
		{
			uint64 v;
			if ( variant.get(v) ) { lua_pushnumber(_L, (lua_Number) v); return true; }
		}
	break;
	case Arcade::VT_INT:
		{
			int64 v;
			if ( variant.get(v) ) { lua_pushnumber(_L, (lua_Number) v); return true; }
		}
	break;
	case Arcade::VT_DOUBLE:
		{
			double v;
			if ( variant.get(v) ) { lua_pushnumber(_L, (lua_Number) v); return true; }
		}
	break;
	case Arcade::VT_STRING:
		{
			CString str;
			if ( variant.get(str) ) { lua_pushstring(_L, *str); return true; }
		}
	break;
	default:
		lua_pushnil(_L);
		return true;
	break;
	}

	return false;
}

weak_ptr<IVMClass> CLuaVM::loadGameVMClass(shared_ptr<CPackage> gamePackage)
{
	shared_ptr<CLuaVMClass> newClass(nullptr);

	auto found = _loadedClasses.find(gamePackage->getPackageID());
	if ( found != _loadedClasses.end() )
	{
		(*found).second->loadFromPackage(gamePackage);

		return (*found).second;
	}
	else
	{
		newClass = makeShared<CLuaVMClass>(shared_from_this());
		_loadedClasses.insert(make_pair(gamePackage->getPackageID(), newClass));

		if ( newClass->loadFromPackage(gamePackage) )
		{
			return newClass;
		}
	}

	return newClass;
}

bool CLuaVM::includeGameScript()
{
	return false;
}

bool CLuaVM::validateGameScript()
{
	return false;
}