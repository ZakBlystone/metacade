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

class LuaDrawModule : public LuaDrawModulePublic
{
public:
	static CElementRenderer *get(lua_State *L)
	{
		LuaDrawModule *instance = GET_OBJECT(LuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return nullptr;
		return instance->_renderer.get();
	}

	MODULE_FUNCTION_DEF(color)
	{
		LuaDrawModule *instance = GET_OBJECT(LuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		instance->_currentColor.r = (float)luaL_checknumber(L, 2);
		instance->_currentColor.g = (float)luaL_checknumber(L, 3);
		instance->_currentColor.b = (float)luaL_checknumber(L, 4);
		instance->_currentColor.a = (float)luaL_optnumber(L, 5, 1.f);

		return 0;
	}

	MODULE_FUNCTION_DEF(rect)
	{
		LuaDrawModule *instance = GET_OBJECT(LuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);
		float w = (float)luaL_checknumber(L, 4);
		float h = (float)luaL_checknumber(L, 5);

		CRenderState defState;

		CRenderQuad quad;
		quad.makeBox(CVec2(x,y), CVec2(x+w,y+h), instance->_currentColor);

		CRenderElement& el = instance->_renderer->addRenderElement();
		el.makeQuad(quad, instance->_renderer->getViewportClip(), defState);

		return 0;
	}

	MODULE_FUNCTION_DEF(sprite)
	{
		LuaDrawModule *instance = GET_OBJECT(LuaDrawModule, L, 1);
		if ( instance == nullptr || instance->_renderer == nullptr ) return 0;

		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);
		float w = (float)luaL_checknumber(L, 4) * .5f;
		float h = (float)luaL_checknumber(L, 5) * .5f;
		float r = (float)luaL_optnumber(L, 6, 0);

		CRenderState defState;
		CMatrix3 xform;
		CMatrix3::identity(xform);

		xform.rotate(r);
		xform.translate(CVec2(x,y));

		CRenderQuad quad;
		quad.makeBox(CVec2(-w,-h), CVec2(w,h), instance->_currentColor);
		quad.transform(xform);

		CRenderElement& el = instance->_renderer->addRenderElement();
		el.makeQuad(quad, instance->_renderer->getViewportClip(), defState);

		return 0;
	}

	DEFINE_MODULE(LuaDrawModule, Drawer)
	MODULE_FUNCTION(color)
	MODULE_FUNCTION(rect)
	MODULE_FUNCTION(sprite)
	END_DEFINE_MODULE()

	CFloatColor _currentColor;
}; 

CREATE_MODULE(LuaDrawModule)

void Arcade::pushRenderer(lua_State *L, shared_ptr<CElementRenderer> renderer)
{
	PUSH_MODULE(LuaDrawModule, L)->_renderer = renderer;
}

void Arcade::OpenLuaDrawModule(lua_State *L)
{
	OPEN_MODULE(L, LuaDrawModule);
}