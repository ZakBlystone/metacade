/*
* <LuaDraw.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaDraw.h"
#include "LuaMath.h"
#include "Runtime.h"
#include "LuaAssets.h"
#include "GLRenderThread.h"
#include "RenderStack.h"

using namespace Arcade;

const float RAD_2_DEG = 57.29577f;
const float DEG_2_RAD = 1.f / RAD_2_DEG;

class DrawModule : public LuaModule
{
public:
	MODULE_FUNCTION_DEF(Rect)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		auto RC = MAKE_RENDER_COMMAND(Instance->GetRenderStack(), Image);
		RC->Handle = 0;
		RC->X = (float)luaL_checknumber(L, 1);
		RC->Y = (float)luaL_checknumber(L, 2);
		RC->W = (float)luaL_checknumber(L, 3);
		RC->H = (float)luaL_checknumber(L, 4);

		return 0;
	}

	MODULE_FUNCTION_DEF(Image)
	{
		ImageAsset *Asset = lua_toImageAsset(L, 1);
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance || !Asset) return 0;

		auto RC = MAKE_RENDER_COMMAND(Instance->GetRenderStack(), Image);
		RC->Handle = Asset->Handle;
		RC->X = (float)luaL_checknumber(L, 2);
		RC->Y = (float)luaL_checknumber(L, 3);
		RC->W = (float)luaL_optnumber(L, 4, 1.f);
		RC->H = (float)luaL_optnumber(L, 5, RC->W);

		RC->W *= Asset->GetWidth();
		RC->H *= Asset->GetHeight();

		return 0;
	}

	MODULE_FUNCTION_DEF(Sprite)
	{
		ImageAsset *Asset = lua_toImageAsset(L, 1);
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance || !Asset) return 0;

		auto RC = MAKE_RENDER_COMMAND(Instance->GetRenderStack(), Sprite);
		RC->Handle = Asset->Handle;
		RC->X = (float)luaL_checknumber(L, 2);
		RC->Y = (float)luaL_checknumber(L, 3);
		RC->R = (float)luaL_optnumber(L, 4, 0.f) * DEG_2_RAD;
		RC->W = (float)luaL_optnumber(L, 5, 1.f);
		RC->H = (float)luaL_optnumber(L, 6, RC->W);

		RC->W *= Asset->GetWidth();
		RC->H *= Asset->GetHeight();

		return 0;
	}

	MODULE_FUNCTION_DEF(Clear)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		auto RC = MAKE_RENDER_COMMAND(Instance->GetRenderStack(), Clear);
		RC->R = (float)luaL_optnumber(L, 1, 0.f);
		RC->G = (float)luaL_optnumber(L, 2, 0.f);
		RC->B = (float)luaL_optnumber(L, 3, 0.f);
		RC->A = (float)luaL_optnumber(L, 4, 1.f);

		return 0;
	}

	MODULE_FUNCTION_DEF(Color)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		auto RC = MAKE_RENDER_COMMAND(Instance->GetRenderStack(), SetColor);
		RC->R = (float)luaL_optnumber(L, 1, 1.f);
		RC->G = (float)luaL_optnumber(L, 2, 1.f);
		RC->B = (float)luaL_optnumber(L, 3, 1.f);
		RC->A = (float)luaL_optnumber(L, 4, 1.f);

		return 0;
	}

	MODULE_FUNCTION_DEF(Push)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		MAKE_RENDER_COMMAND(Instance->GetRenderStack(), PushTransform)
		return 0;
	}

	MODULE_FUNCTION_DEF(Pop)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		MAKE_RENDER_COMMAND(Instance->GetRenderStack(), PopTransform)
		return 0;
	}

	MODULE_FUNCTION_DEF(Identity)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		MAKE_RENDER_COMMAND(Instance->GetRenderStack(), Identity)
		return 0;
	}

	MODULE_FUNCTION_DEF(Transform)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		auto RC = MAKE_RENDER_COMMAND(Instance->GetRenderStack(), MatrixMult);

		float *Mtx = lua_toMatrix3x3(L, 1);
		if (!Mtx) return 0;

		memcpy(RC->Mtx, Mtx, sizeof(float) * 9);
		return 0;
	}

	MODULE_FUNCTION_DEF(BlendAdd)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		MAKE_RENDER_COMMAND(Instance->GetRenderStack(), BlendAdd)
		return 0;
	}

	MODULE_FUNCTION_DEF(BlendMultiply)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		MAKE_RENDER_COMMAND(Instance->GetRenderStack(), BlendMultiply)
		return 0;
	}

	MODULE_FUNCTION_DEF(BlendNormal)
	{
		Game *Instance = getUserDataStatic<DrawModule, Game>(L);
		if (!Instance) return 0;

		MAKE_RENDER_COMMAND(Instance->GetRenderStack(), BlendNormal)
		return 0;
	}

	DEFINE_MODULE(DrawModule, Draw)
	MODULE_FUNCTION(Rect)
	MODULE_FUNCTION(Image)
	MODULE_FUNCTION(Sprite)
	MODULE_FUNCTION(Clear)
	MODULE_FUNCTION(Color)
	MODULE_FUNCTION(Push)
	MODULE_FUNCTION(Pop)
	MODULE_FUNCTION(Identity)
	MODULE_FUNCTION(Transform)
	MODULE_FUNCTION(BlendAdd)
	MODULE_FUNCTION(BlendMultiply)
	MODULE_FUNCTION(BlendNormal)
	END_DEFINE_MODULE()
}; CREATE_MODULE(DrawModule);


void OpenLuaDrawModule(lua_State *L, Game *GameInstance)
{
	OPEN_MODULE_USERDATA(L, DrawModule, GameInstance);
}
