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
lua_module.h:
===============================================================================
*/

#pragma once

#define DEFINE_MODULE(classname,name) static const char *__name; \
	static unsigned int __size; \
	static const char *__class; \
	virtual const char *__getName() {return __name;}; \
	virtual const char *__getClass() {return __class;}; \
	virtual void __setClass(const char *cl) {__class = cl;}; \
	virtual unsigned int __getSize() {return __size;}; \
	virtual void open() {__name = #name; __size = sizeof(*this); __constructor = NULL;

#define MODULE_FUNCTION(f) this->__closures[#f] = f;
#define MODULE_FUNCTION_DEF(f) static int f(lua_State *L)
#define MODULE_CONSTRUCTOR(f) this->__constructor = f;
#define END_DEFINE_MODULE() }
#define CREATE_MODULE(name) name* m_##name = new name(); \
	const char *name::__name;\
	const char *name::__class;\
	unsigned int name::__size;
#define OPEN_MODULE(L,name) _loadLuaModule(L,m_##name,#name,nullptr)
#define OPEN_MODULE_USERDATA(L,name,arg) _loadLuaModule(L,m_##name,#name,arg)
#define GET_MODULE(name) m_##name
#define GET_OBJECT(name,state,idx) (name*)LuaModule::get(state,#name,idx)
#define PUSH_TVALUE(state,t) lua_rawgeti(L, LUA_REGISTRYINDEX, t)
#define PUSH_MODULE(name,state) ((name *)(m_##name->push(state)))

namespace Arcade
{

class LuaModule
{
public:
	void *__userdata;
	bool __staticAccess;
	std::map<const char *, lua_CFunction> __closures;
	int __table;
	lua_CFunction __constructor;
	virtual void open() {};
	virtual const char *__getName() { return "_abstract_"; }
	virtual const char *__getClass() { return "_abstract_"; }
	virtual unsigned int __getSize() { return 0; }
	virtual void __setClass(const char *cl) { cl = nullptr; }
	virtual void __set(lua_State *L) { L = nullptr; }
	virtual void __get(lua_State *L) { L = nullptr; }
	LuaModule *push(lua_State *L);
	void getTable(lua_State *L);
	static LuaModule *get(lua_State *L, const char *classname, int idx);

	template<typename CALLER, typename T> static T* getUserDataStatic(lua_State *L)
	{
		lua_getglobal(L, CALLER::__name);
		int udata = lua_gettop(L);
		luaL_checktype(L, udata, LUA_TUSERDATA);
		LuaModule *m = (LuaModule *)luaL_checkudata(L, udata, CALLER::__name);
		if (!m) return nullptr;

		lua_pop(L, 1);

		return static_cast<T*>(m->__userdata);
	}
};

extern void _loadLuaModule(lua_State *L, LuaModule *m, const char *classname, void *userdata);

}