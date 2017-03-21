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
lua_draw.cpp:
===============================================================================
*/

#include "lua_private.h"
#include "render/render_private.h"

class CLuaDrawModule : public LuaDrawModulePublic
{
public:
	static CElementRenderer *get(lua_State *L)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return nullptr;
		return instance->_renderer.get();
	}

	MODULE_FUNCTION_DEF(color)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		instance->_currentColor.r = (float)luaL_checknumber(L, 2);
		instance->_currentColor.g = (float)luaL_checknumber(L, 3);
		instance->_currentColor.b = (float)luaL_checknumber(L, 4);
		instance->_currentColor.a = (float)luaL_optnumber(L, 5, 1.f);

		return 0;
	}

	MODULE_FUNCTION_DEF(rect)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);
		float w = (float)luaL_checknumber(L, 4);
		float h = (float)luaL_checknumber(L, 5);
		uint32 t = (uint32)luaL_optnumber(L, 6, 0);

		CRenderState defState;

		defState._material._baseTexture = t;

		CRenderQuad quad;
		quad.makeBox(CVec2(x,y), CVec2(x+w,y+h), instance->_currentColor);

		CRenderElement& el = instance->_renderer->addRenderElement();
		el.makeQuad(quad, instance->_renderer->getViewportClip(), defState, instance->_layer);

		return 0;
	}

	MODULE_FUNCTION_DEF(sprite)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);
		float w = (float)luaL_checknumber(L, 4) * .5f;
		float h = (float)luaL_checknumber(L, 5) * .5f;
		float r = (float)luaL_optnumber(L, 6, 0);
		uint32 t = (uint32)luaL_optnumber(L, 7, 0);

		CRenderState defState;
		CMatrix3 xform;
		CMatrix3::identity(xform);

		defState._material._baseTexture = t;

		xform.rotate(r);
		xform.translate(CVec2(x,y));

		CRenderQuad quad;
		quad.makeBox(CVec2(-w,-h), CVec2(w,h), instance->_currentColor);
		quad.transform(xform);

		CRenderElement& el = instance->_renderer->addRenderElement();
		el.makeQuad(quad, instance->_renderer->getViewportClip(), defState, instance->_layer);

		return 0;
	}

	MODULE_FUNCTION_DEF(quad)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		CRenderQuad quad;

		for ( uint32 i=0; i<4; ++i )
		{
			quad._verts[i]._position.x = (float)luaL_checknumber(L, i*4 + 2);
			quad._verts[i]._position.y = (float)luaL_checknumber(L, i*4 + 3);
			quad._verts[i]._texcoord.x = (float)luaL_checknumber(L, i*4 + 4);
			quad._verts[i]._texcoord.y = (float)luaL_checknumber(L, i*4 + 5);
			quad._verts[i]._color = instance->_currentColor;
		}

		uint32 t = (uint32)luaL_optnumber(L, 18, 0);

		CRenderState defState;
		defState._material._baseTexture = t;

		CRenderElement& el = instance->_renderer->addRenderElement();
		el.makeQuad(quad, instance->_renderer->getViewportClip(), defState, instance->_layer);

		return 0;
	}

	MODULE_FUNCTION_DEF(size)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		lua_pushnumber(L, instance->_renderer->getViewSize().x);
		lua_pushnumber(L, instance->_renderer->getViewSize().y);
		return 2;
	}

	MODULE_FUNCTION_DEF(layer)
	{
		CLuaDrawModule *instance = GET_OBJECT(CLuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		instance->_layer = (int32) lua_tointeger(L, 2);
		return 0;
	}

	DEFINE_MODULE(CLuaDrawModule, Drawer)
	MODULE_FUNCTION(color)
	MODULE_FUNCTION(rect)
	MODULE_FUNCTION(sprite)
	MODULE_FUNCTION(quad)
	MODULE_FUNCTION(size)
	MODULE_FUNCTION(layer)
	END_DEFINE_MODULE()

	int32 _layer;
	CFloatColor _currentColor;
}; 

CREATE_MODULE(CLuaDrawModule)

void Arcade::pushRenderer(lua_State *L, shared_ptr<CElementRenderer> renderer)
{
	CLuaDrawModule* drawModule = PUSH_MODULE(CLuaDrawModule, L);
	drawModule->_renderer = renderer;
	drawModule->_layer = 0;
}

void Arcade::OpenLuaDrawModule(lua_State *L)
{
	OPEN_MODULE(L, CLuaDrawModule);
}