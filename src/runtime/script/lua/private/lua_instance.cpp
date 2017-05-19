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

static void copyTable(lua_State* L, int32 order = 0)
{
	lua_pushnil(L); //-1 nil, -2 src, -3 dst

	while (lua_next(L, -2))
	{
		//-1 v, -2 k, -3 src, -4 dst
		if ( lua_type(L, -1) != LUA_TTABLE )
		{
			lua_pushvalue(L, -2); //-1 ck, -2 v, -3 k, -4 src, -5 dst
			lua_pushvalue(L, -2); //-1 cv, -2 ck, -3 v, -4 k, -5 src, -6 dst

			//for ( int32 i=0; i<order; ++i ) std::cout << "\t";
			//std::cout << lua_tostring(L, -2) << std::endl;

			lua_rawset(L, -6); //-1 v, -2 k, -3 src, -4 dst
		}
		else if ( order < 4 && ( lua_type(L, -2) != LUA_TSTRING || strcmp(lua_tostring(L, -2), "_G") ) )
		{
			//for ( int32 i=0; i<order; ++i ) std::cout << "\t";
			//std::cout << "---" << lua_tostring(L, -2) << std::endl;

			lua_newtable(L); //-1 t, -2 v, -3 k, -4 src, -5 dst
			lua_pushvalue(L, -2); //-1 cv, -2 t, -3 v, -4 k, -5 src, -6 dst
			copyTable(L, order+1); //-1 t, -2 v, -3 k, -4 src, -5 dst

			lua_pushvalue(L, -3); //-1 ck, -2 t, -3 v, -4 k, -5 src, -6 dst
			lua_pushvalue(L, -2); //-1 t, -2 ck, -3 t, -4 v, -5 k, -6 src, -7 dst
			lua_rawset(L, -7); //-1 t, -2 v, -3 k, -4 src, -5 dst
			lua_pop(L, 1); //-1 v, -2 k, -3 src, -4 dst
		}
		lua_pop(L, 1); //-1 k, -2 src, -3 dst
	}

	lua_pop(L, 1); //-1 dst
}

static const char* G_blacklist[] =
{
	"setfenv",
	"getfenv",
	"dofile",
	"load",
	"loadstring",
	"loadfile",
	0,
};

void CLuaVMInstance::createAssetRefTable(EAssetType type, const CString& prefix)
{
	CPackage* pkg = getLuaClass()->getPackage();
	lua_State* L = getLuaClass()->_host->_L;

	lua_newtable(L);

	for ( uint32 i=0; i<pkg->getNumAssets(); ++i )
	{
		CAssetRef ref = pkg->getAsset(i);
		if ( ref.getType() != type ) continue;

		IAsset* asset = ref.get(getRuntime());
		if ( asset == nullptr ) continue;

		pushAssetRef(L, ref);

		CString name = asset->getName();
		int32 dot = name.find(".");
		if ( dot != -1 )
		{
			name = name.sub(0, dot);
		}

		lua_setfield(L, -2, *name);
	}

	lua_setfield(L, -2, *prefix);
}

//VM INSTANCE
Arcade::CLuaVMInstance::CLuaVMInstance(weak_ptr<CLuaVMClass> klass)
	: _klass(klass)
	, CRuntimeObject(klass.lock().get())
{
	if ( _klass.expired() ) return;

	CPackage* pkg = getLuaClass()->getPackage();
	lua_State* L = getLuaClass()->_host->_L;

	if ( pkg == nullptr ) return;

	lua_newtable(L);
	lua_getglobal(L, "_G");
	copyTable(L); //copy everything from _G to the local table

	lua_pushstring(L, "game");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	createAssetRefTable(ASSET_TEXTURE, "_t");
	createAssetRefTable(ASSET_SOUND, "_s");

	const char** ptr = G_blacklist;
	while (*ptr)
	{
		lua_pushstring(L, *ptr);
		lua_pushnil(L);
		lua_settable(L, -3);
		++ptr;
	}

	_object = make_shared<LuaVMReference>(getLuaClass()->_host, -1);

	for ( auto funcdef : getLuaClass()->_functions )
	{
		funcdef.second->push();
		lua_setfield(L, -2, *funcdef.first);
	}

	lua_pop(L, 1);
}

Arcade::CLuaVMInstance::~CLuaVMInstance()
{

}

class IVMClass* Arcade::CLuaVMInstance::getClass()
{
	if ( _klass.expired() ) return nullptr;

	return getLuaClass();
}

void Arcade::CLuaVMInstance::setGameInstance(IGameInstance* gameInstance)
{
	lua_State* L = getLuaHost()->getState();

	_gameInstance = gameInstance;
	_object->push();
	lua_pushlightuserdata(L, _gameInstance);
	lua_setfield(L, -2, "__gameinstance");
	lua_pop(L, 1);
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

void Arcade::CLuaVMInstance::init()
{
	callFunction(CFunctionCall("init"));
}

void Arcade::CLuaVMInstance::think(float seconds, float deltaSeconds)
{
	if ( _klass.expired() ) return;

	callFunction(CFunctionCall("think", seconds, deltaSeconds));
}

void Arcade::CLuaVMInstance::render(shared_ptr<CElementRenderer> renderer)
{
	if ( _klass.expired() ) return;

	lua_State *L = getLuaHost()->_L;

	beginLuaDraw(L, renderer);

	callFunction(CFunctionCall("draw"));

	endLuaDraw(L, renderer);
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

	_object->push();
	lua_getfield(L, -1, "__gameinstance");
	lua_setglobal(L, "__gameinstance");

	lua_setfenv(L, -2);

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

	return getLuaHost()->pcall(nargs);
}