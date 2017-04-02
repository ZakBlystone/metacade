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

//#define DRAW_PASSTHROUGH 1

struct CDrawData
{
	CClipShape _viewClip;
	CFloatColor _currentColor;
	shared_ptr<CElementRenderer> _renderer;
	int32 _layer;

	inline bool valid()
	{
		return _renderer.get() != nullptr;
	}

	inline CElementRenderer* getRenderer()
	{
		return _renderer.get();
	}
};

static CDrawData gData;

void Arcade::beginLuaDraw(lua_State *L, shared_ptr<CElementRenderer> renderer)
{
	gData._renderer = renderer;
	gData._layer = 0;
	gData._currentColor = CFloatColor(1.f, 1.f, 1.f, 1.f);
	gData._viewClip = renderer->getViewportClip();
}

void Arcade::endLuaDraw(lua_State *L, shared_ptr<CElementRenderer> renderer)
{
	gData._renderer = nullptr;
}

MODULE_FUNCTION_DEF(draw_color)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	gData._currentColor.r = (float)luaL_checknumber(L, 1);
	gData._currentColor.g = (float)luaL_checknumber(L, 2);
	gData._currentColor.b = (float)luaL_checknumber(L, 3);
	gData._currentColor.a = (float)luaL_optnumber(L, 4, 1.f);

	return 0;
}

MODULE_FUNCTION_DEF(draw_rect)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	float w = (float)luaL_checknumber(L, 3);
	float h = (float)luaL_checknumber(L, 4);
	uint32 t = (uint32)luaL_optnumber(L, 5, 0);

	CRenderState defState;

	defState._material._baseTexture = t;

	CRenderElement& el = gData.getRenderer()->addRenderElement();

	el.makeQuad2(gData._viewClip, defState, gData._layer)
		.makeBox(CVec2(x,y), CVec2(x+w,y+h), gData._currentColor);

	return 0;
}

MODULE_FUNCTION_DEF(draw_sprite)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	float w = (float)luaL_checknumber(L, 3) * .5f;
	float h = (float)luaL_checknumber(L, 4) * .5f;
	float r = (float)luaL_optnumber(L, 5, 0);
	uint32 t = (uint32)luaL_optnumber(L, 6, 0);

	CRenderState defState;
	CMatrix3 xform;
	CMatrix3::identity(xform);

	defState._material._baseTexture = t;

	xform.rotate(r);
	xform.translate(CVec2(x,y));

	CRenderQuad quad;
	quad.makeBox(CVec2(-w,-h), CVec2(w,h), gData._currentColor);
	quad.transform(xform);

	CRenderElement& el = gData.getRenderer()->addRenderElement();
	el.makeQuad(quad, gData._viewClip, defState, gData._layer);

	return 0;
}

MODULE_FUNCTION_DEF(draw_quad)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	CRenderQuad quad;

	for ( uint32 i=0; i<4; ++i )
	{
		quad._verts[i]._position.x = (float)luaL_checknumber(L, i*4 + 1);
		quad._verts[i]._position.y = (float)luaL_checknumber(L, i*4 + 2);
		quad._verts[i]._texcoord.x = (float)luaL_checknumber(L, i*4 + 3);
		quad._verts[i]._texcoord.y = (float)luaL_checknumber(L, i*4 + 4);
		quad._verts[i]._color = gData._currentColor;
	}

	uint32 t = (uint32)luaL_optnumber(L, 17, 0);

	CRenderState defState;
	defState._material._baseTexture = t;

	CRenderElement& el = gData.getRenderer()->addRenderElement();
	el.makeQuad(quad, gData._viewClip, defState, gData._layer);

	return 0;
}

MODULE_FUNCTION_DEF(draw_size)
{
	lua_pushnumber(L, gData.getRenderer()->getViewSize().x);
	lua_pushnumber(L, gData.getRenderer()->getViewSize().y);
	return 2;
}

MODULE_FUNCTION_DEF(draw_layer)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	gData._layer = (int32) lua_tointeger(L, 1);
	return 0;
}


static const luaL_Reg drawlib[] = {
	{"color", draw_color},
	{"rect", draw_rect},
	{"sprite", draw_sprite},
	{"quad", draw_quad},
	{"size", draw_size},
	{"layer", draw_layer},
	{nullptr, nullptr}
};

void Arcade::OpenLuaDrawModule(lua_State *L)
{
	//OPEN_MODULE(L, CLuaDrawModule);

	luaL_register(L, "_r", drawlib);
	//return 1;
}