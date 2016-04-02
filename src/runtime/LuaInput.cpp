/*
* <LuaInput.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaInput.h"
#include "Runtime.h"

using namespace Arcade;

class InputModule : public LuaModule
{
public:
	static int Joystick(lua_State *L);
	static int Button(lua_State *L);

	MODULE_FUNCTION_DEF(Pressed)
	{
		Game *Instance = getUserDataStatic<InputModule, Game>(L);
		if (!Instance) return 0;

		lua_pushboolean(L, Instance->GetControllerEventState(luaL_checkstring(L, 1)) == CSTATE_PRESSED);

		return 1;
	}

	MODULE_FUNCTION_DEF(Down)
	{
		Game *Instance = getUserDataStatic<InputModule, Game>(L);
		if (!Instance) return 0;

		ControlEventState State = Instance->GetControllerEventState(luaL_checkstring(L, 1));

		lua_pushboolean(L, State == CSTATE_DOWN || State == CSTATE_PRESSED);

		return 1;
	}

	MODULE_FUNCTION_DEF(Released)
	{
		Game *Instance = getUserDataStatic<InputModule, Game>(L);
		if (!Instance) return 0;

		lua_pushboolean(L, Instance->GetControllerEventState(luaL_checkstring(L, 1)) == CSTATE_RELEASED);

		return 1;
	}

	DEFINE_MODULE(InputModule, Input)
	MODULE_FUNCTION(Joystick)
	MODULE_FUNCTION(Button)
	MODULE_FUNCTION(Pressed)
	MODULE_FUNCTION(Down)
	MODULE_FUNCTION(Released)
	END_DEFINE_MODULE()
}; CREATE_MODULE(InputModule);

int InputModule::Joystick(lua_State *L)
{
	return 0;
}

int InputModule::Button(lua_State *L)
{
	return 0;
}

void OpenLuaInputModule(lua_State *L, Game *GameInstance)
{
	OPEN_MODULE_USERDATA(L, InputModule, GameInstance);
}
