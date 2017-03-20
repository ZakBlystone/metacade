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
lua_reference.cpp:
===============================================================================
*/

#include "lua_private.h"

Arcade::LuaVMReference::LuaVMReference(shared_ptr<class LuaVM> host, int idx)
	: _host(host)
{
	lua_State *L = host->_L;
	
	lua_pushvalue(L, idx);
	_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

Arcade::LuaVMReference::~LuaVMReference()
{
	lua_State *L = _host->_L;

	if ( L == nullptr ) return;

	luaL_unref(L, LUA_REGISTRYINDEX, _ref);
}

void Arcade::LuaVMReference::push()
{
	lua_State *L = _host->_L;

	if ( L == nullptr ) return;

	lua_rawgeti(L, LUA_REGISTRYINDEX, _ref);
}
