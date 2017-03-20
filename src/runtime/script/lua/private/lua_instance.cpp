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
lua_instance.cpp:
===============================================================================
*/

#include "lua_private.h"

//VM INSTANCE
Arcade::LuaVMInstance::LuaVMInstance(shared_ptr<LuaVMClass> klass)
	: _klass(klass)
{
	lua_State *L = _klass->_host->_L;

	lua_newtable(L);
	_object = make_shared<LuaVMReference>(_klass->_host, -1);

	for ( auto funcdef : klass->_functions )
	{
		funcdef.second->push();
		lua_setfield(L, -2, funcdef.first.c_str());
	}

	lua_pop(L, 1);

	if ( getLuaClass()->pushLuaFunction("init") ) pcall(0);
}

Arcade::LuaVMInstance::~LuaVMInstance()
{

}

class IVMClass* Arcade::LuaVMInstance::getClass()
{
	return _klass.get();
}

void Arcade::LuaVMInstance::setMachineEnvironment(IMachineEnvironment *env)
{

}

bool Arcade::LuaVMInstance::postCommand(const char** commandBuffer)
{
	return false;
}

void Arcade::LuaVMInstance::postInputEvent(const class CInputEvent& input)
{

}

void Arcade::LuaVMInstance::precacheAssets(CPackageBuilder* builder)
{

}

void Arcade::LuaVMInstance::think(float seconds, float deltaSeconds)
{
	if ( getLuaClass()->pushLuaFunction("think") )
	{
		lua_State *L = getLuaHost()->_L;

		lua_pushnumber(L, seconds);
		lua_pushnumber(L, deltaSeconds);
		pcall(2);
	}
}

void Arcade::LuaVMInstance::render(shared_ptr<CElementRenderer> renderer)
{
	if ( getLuaClass()->pushLuaFunction("draw") ) 
	{
		lua_State *L = getLuaHost()->_L;

		pushRenderer(L, renderer);
		lua_setglobal(L, "_r");

		pcall(0);

		lua_pushnil(L);
		lua_setglobal(L, "_r");
	}
}

void Arcade::LuaVMInstance::reset()
{
	if ( getLuaClass()->pushLuaFunction("reset") ) pcall(0);
}

bool LuaVMInstance::callFunction(CFunctionCall call)
{
	lua_State *L = getLuaHost()->_L;

	if ( !getLuaClass()->pushLuaFunction(*call.getFunction()) ) 
	{
		return false;
	}

	int top = lua_gettop(L);

	for ( uint32 i=0; i<call.numArgs(); ++i )
	{
		if ( !getLuaHost()->pushVariant(call.getArg(i)) )
		{
			lua_settop(L, top);
			return false;
		}
	}

	return pcall(call.numArgs());
}

LuaVM* Arcade::LuaVMInstance::getLuaHost() const
{
	return _klass->getLuaHost().get();
}

LuaVMClass* Arcade::LuaVMInstance::getLuaClass() const
{
	return _klass.get();
}

LuaVMReference* Arcade::LuaVMInstance::getLuaObject() const
{
	return _object.get();
}

bool Arcade::LuaVMInstance::pcall(int nargs)
{
	getLuaObject()->push();
	lua_setglobal(getLuaHost()->_L, "game");
	return getLuaHost()->pcall(nargs);
}