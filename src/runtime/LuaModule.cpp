/*
* <LuaModule.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaModule.h"
#include "Runtime.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace Arcade;

struct cmp_str
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

static std::map<const char *, LuaModule *, cmp_str> __modules;

LuaModule *LuaModule::push(lua_State *L)
{
	int t;
	//int lmsize = sizeof(LuaModule);
	LuaModule *m = NULL;
	m = (LuaModule*)lua_newuserdata(L, __getSize());

	if (!m) return NULL;

	memcpy(m, this, __getSize());

	t = lua_gettop(L);

	luaL_getmetatable(L, __getName());
	lua_setmetatable(L, -2);

	lua_newtable(L);
	m->__table = luaL_ref(L, LUA_REGISTRYINDEX);
	m->__set(L);

	lua_settop(L, t);

	return m;
}

LuaModule *LuaModule::get(lua_State *L, const char *classname, int idx)
{
	LuaModule *classz;
	auto iter = __modules.find(classname);
	if (iter == __modules.end())
	{
		Runtime::LogPrint(LOG_ERROR, "Attempted to access unloaded module: %s", classname);
		return nullptr;
	}

	classz = (*iter).second;
	if (classz == nullptr)
	{
		Runtime::LogPrint(LOG_ERROR, "module %s was null", classname);
		return nullptr;
	}

	LuaModule *m = NULL;
	luaL_checktype(L, idx, LUA_TUSERDATA);
	m = (LuaModule *)luaL_checkudata(L, idx, classz->__getName());

	m->__get(L);

	return m;
}

void LuaModule::getTable(lua_State *L)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, __table);
}

static int luaMetaSet(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TUSERDATA);
	void *ud = lua_touserdata(L, 1);
	LuaModule *m = (LuaModule*)ud;
	const char *key = lua_tostring(L, 2);
	//const char *classz = m->__getClass();
	luaL_getmetatable(L, m->__getName());

	//std::cout << "SET: " << key << " TYPE: " << lua_type(L,3) << "\n";

	//lua_pushliteral(L, "__mtx");
	//lua_rawget(L, -2);
	lua_rawgeti(L, LUA_REGISTRYINDEX, m->__table);

	lua_pushstring(L, key);
	lua_pushvalue(L, 3);
	lua_rawset(L, -3);

	return 0;
}

static int luaMetaAccess(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TUSERDATA);
	void *ud = lua_touserdata(L, 1);
	LuaModule *m = (LuaModule*)ud;
	const char *key = lua_tostring(L, 2);
	//const char *classz = m->__getClass();
	luaL_getmetatable(L, m->__getName());

	lua_pushliteral(L, "__mtx");
	lua_rawget(L, -2);

	lua_pushstring(L, key);
	lua_rawget(L, -2);

	if (lua_type(L, -1) != LUA_TNIL)
	{
		return 1;
	}
	lua_pop(L, 2);

	lua_rawgeti(L, LUA_REGISTRYINDEX, m->__table);
	lua_pushstring(L, key);
	lua_rawget(L, -2);

	if (lua_type(L, lua_gettop(L)) != LUA_TNIL)
	{
		return 1;
	}

	return 0;
}

static int gc(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TUSERDATA);
	void *ud = lua_touserdata(L, 1);
	LuaModule *m = (LuaModule*)ud;

	luaL_unref(L, LUA_REGISTRYINDEX, m->__table);
	//std::cout << "GC: " << m->__getName() << "\n";

	return 0;
}

#define DEBUG_TABLES 1
void _loadLuaModule(lua_State *L, LuaModule *m, const char *classname, void *userdata)
{
	m->open();
	std::map<const char *, lua_CFunction>::iterator iter = m->__closures.begin();

	m->__setClass(classname);
	__modules[classname] = m;

	luaL_newmetatable(L, m->__getName());

	//if(m->__daVars.size())
	//{
	//std::cout << "DA\n";
	lua_pushliteral(L, "__mtx");
	lua_pushvalue(L, -2);
	lua_rawset(L, -3);

	lua_pushliteral(L, "__index");
	lua_pushcclosure(L, luaMetaAccess, 0);
	lua_rawset(L, -3);

	lua_pushliteral(L, "__newindex");
	lua_pushcclosure(L, luaMetaSet, 0);
	lua_rawset(L, -3);

	lua_pushliteral(L, "__gc");
	lua_pushcclosure(L, gc, 0);
	lua_rawset(L, -3);
	//} else {
	//std::cout << "NDA\n";
	//lua_pushliteral(L, "__index");
	//lua_pushvalue(L, -2);
	//lua_rawset(L, -3);		
	//}

	std::cout << "\nOPEN MODULE: " << m->__getName() << " = " << classname << "\n";
	while (iter != m->__closures.end())
	{
		std::pair<const char *, lua_CFunction> f = *iter;
		//std::cout << f.first << " = " << f.second << "\n";

		lua_pushcclosure(L, f.second, 0);
		lua_setfield(L, -2, f.first);

		iter++;
	}

	std::string str = "__t";
	str.append(m->__getName());
	if (m->__constructor)
	{
		m->__staticAccess = false;
		lua_register(L, m->__getName(), m->__constructor);

		if (DEBUG_TABLES)
		{
			m->push(L);
			lua_setglobal(L, str.c_str());
		}
	}
	else
	{
		m->__staticAccess = true;
		LuaModule *Loaded = m->push(L);

		if (Loaded)
		{
			Loaded->__userdata = userdata;
		}

		lua_setglobal(L, m->__getName());

		if (DEBUG_TABLES) lua_setglobal(L, str.c_str());
	}

	//lua_pop(L,1);
}
