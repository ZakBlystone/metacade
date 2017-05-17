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
Arcade::CLuaVMInstance::CLuaVMInstance(weak_ptr<CLuaVMClass> klass)
	: _klass(klass)
{
	if ( _klass.expired() ) return;

	lua_State *L = getLuaClass()->_host->_L;

	lua_newtable(L);
	_object = make_shared<LuaVMReference>(getLuaClass()->_host, -1);

	for ( auto funcdef : getLuaClass()->_functions )
	{
		funcdef.second->push();
		lua_setfield(L, -2, funcdef.first.c_str());
	}

	lua_pop(L, 1);

	if ( getLuaClass()->pushLuaFunction("init") ) 
	{
		pcall(0);
	}
}

Arcade::CLuaVMInstance::~CLuaVMInstance()
{

}

class IVMClass* Arcade::CLuaVMInstance::getClass()
{
	if ( _klass.expired() ) return nullptr;

	return getLuaClass();
}

void Arcade::CLuaVMInstance::setMachineEnvironment(IMachineEnvironment *env)
{

}

void Arcade::CLuaVMInstance::postInputEvent(const class CInputEvent& input)
{
	bool success = false;
	float mouseX;
	float mouseY;

	_state.applyEvent(input);
	_state.getMousePosition(mouseX, mouseY);

	switch ( input.getEventType() )
	{
	case INPUTEVENT_NONE:
	break;
	case INPUTEVENT_MOUSEPRESSED:
		success = callFunction(CFunctionCall("onMousePressed"
			, mouseX
			, mouseY
			, (int32) input.getMouseButton()));
	break;
	case INPUTEVENT_MOUSERELEASED:
		success = callFunction(CFunctionCall("onMouseReleased"
			, mouseX
			, mouseY
			, (int32) input.getMouseButton()));
	break;
	case INPUTEVENT_MOUSEMOVED:
		success = callFunction(CFunctionCall("onMouseMoved"
			, input.getMouseX()
			, input.getMouseY()
			, input.getMouseDeltaX()
			, input.getMouseDeltaY()));
	break;
	case INPUTEVENT_KEYPRESSED:
		success = callFunction(CFunctionCall("onKeyPressed", (int32) input.getKeycode()));
	break;
	case INPUTEVENT_KEYRELEASED:
		success = callFunction(CFunctionCall("onKeyReleased", (int32) input.getKeycode()));
	break;
	case INPUTEVENT_FOCUSGAINED:
		success = callFunction(CFunctionCall("onFocusGained", (int32) input.getFocusElement()));
	break;
	case INPUTEVENT_FOCUSLOST:
		success = callFunction(CFunctionCall("onFocusLost", (int32) input.getFocusElement()));
	break;
	default:
	break;
	}
}

void Arcade::CLuaVMInstance::think(float seconds, float deltaSeconds)
{
	if ( _klass.expired() ) return;

	if ( getLuaClass()->pushLuaFunction("think") )
	{
		lua_State *L = getLuaHost()->_L;

		lua_pushnumber(L, seconds);
		lua_pushnumber(L, deltaSeconds);
		pcall(2);
	}
}

void Arcade::CLuaVMInstance::render(shared_ptr<CElementRenderer> renderer)
{
	if ( _klass.expired() ) return;

	if ( getLuaClass()->pushLuaFunction("draw") ) 
	{
		lua_State *L = getLuaHost()->_L;

		beginLuaDraw(L, renderer);
		//lua_setglobal(L, "_r");

		pcall(0);

		endLuaDraw(L, renderer);
		//lua_pushnil(L);
		//lua_setglobal(L, "_r");
	}
}

void Arcade::CLuaVMInstance::reset()
{
	callFunction(CFunctionCall("reset"));
}

bool CLuaVMInstance::callFunction(const CFunctionCall& call)
{
	if ( _klass.expired() ) return false;

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

CLuaVM* Arcade::CLuaVMInstance::getLuaHost() const
{
	if ( _klass.expired() ) return nullptr;

	return getLuaClass()->getLuaHost().get();
}

CLuaVMClass* Arcade::CLuaVMInstance::getLuaClass() const
{
	auto klass = _klass.lock();
	return klass.get();
}

LuaVMReference* Arcade::CLuaVMInstance::getLuaObject() const
{
	return _object.get();
}

bool Arcade::CLuaVMInstance::pcall(int nargs)
{
	if ( _klass.expired() ) return false;

	getLuaObject()->push();
	lua_setglobal(getLuaHost()->_L, "game");
	return getLuaHost()->pcall(nargs);
}