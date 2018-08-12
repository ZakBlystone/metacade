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
static CDrawInterface gDraw;

static CImageAsset* opttexture(lua_State* L, int idx)
{
	if ( lua_gettop(L) >= idx )
	{
		IAsset* asset = toAsset(L, idx);
		if ( asset == nullptr ) return nullptr;
		if ( asset->getType() != EAssetType::ASSET_TEXTURE ) return nullptr;

		return (CImageAsset*) (asset);
	}
	return nullptr;
}

void Arcade::beginLuaDraw(lua_State *, shared_ptr<CElementRenderer> renderer)
{
	gDraw.setRenderer( renderer );
	gDraw.reset();
}

void Arcade::endLuaDraw(lua_State *, shared_ptr<CElementRenderer> renderer)
{
	gDraw.setRenderer(nullptr);
}

MODULE_FUNCTION_DEF(draw_color)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	static CFloatColor col;
	col.r = (float)luaL_checknumber(L, 1);
	col.g = (float)luaL_checknumber(L, 2);
	col.b = (float)luaL_checknumber(L, 3);
	col.a = (float)luaL_optnumber(L, 4, 1.f);

	gDraw.setColor( col );

	if ( lua_gettop(L) > 4 )
	{
		gDraw.setBlendMode( (EBlendMode) luaL_checkinteger(L, 5) );
	}

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
	CImageAsset* texture = opttexture(L, 5);
	float u0 = (float)luaL_optnumber(L, 6, 0);
	float v0 = (float)luaL_optnumber(L, 7, 0);
	float u1 = (float)luaL_optnumber(L, 8, 1);
	float v1 = (float)luaL_optnumber(L, 9, 1);

	gDraw.drawRect(x,y,w,h,texture,u0,v0,u1,v1);

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
	CImageAsset* texture = opttexture(L, 6);
	float u0 = (float)luaL_optnumber(L, 7, 0);
	float v0 = (float)luaL_optnumber(L, 8, 0);
	float u1 = (float)luaL_optnumber(L, 9, 1);
	float v1 = (float)luaL_optnumber(L, 10, 1);

	gDraw.drawSprite(x,y,w,h,r,texture,u0,v0,u1,v1);

	return 0;
}

MODULE_FUNCTION_DEF(draw_quad)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	CRenderQuad quad;
	CColor color = gDraw.getColor();

	for ( uint32 i=0; i<4; ++i )
	{
		quad._verts[i]._position._x = (float)luaL_checknumber(L, i*4 + 1);
		quad._verts[i]._position._y = (float)luaL_checknumber(L, i*4 + 2);
		quad._verts[i]._texcoord._x = (float)luaL_checknumber(L, i*4 + 3);
		quad._verts[i]._texcoord._y = (float)luaL_checknumber(L, i*4 + 4);
		quad._verts[i]._color._irgba = color._irgba;
	}

	CImageAsset* texture = opttexture(L, 17);
	
	gDraw.drawQuad(quad, texture);

	return 0;
}

MODULE_FUNCTION_DEF(draw_size)
{
	CVec2 size = gDraw.getSize();

	lua_pushnumber(L, size._x);
	lua_pushnumber(L, size._y);
	return 2;
}

MODULE_FUNCTION_DEF(draw_layer)
{
	#ifdef DRAW_PASSTHROUGH 
	return 0; 
	#endif

	gDraw.setLayer( (int32) lua_tointeger(L, 1) );
	return 0;
}


MODULE_FUNCTION_DEF(draw_push)
{
	if ( !gDraw.pushTransform() )
		return luaL_error(L, "Draw matrix stack overflow");

	return 0;
}

MODULE_FUNCTION_DEF(draw_pop)
{
	if ( !gDraw.popTransform() )
		return luaL_error(L, "Draw matrix stack underflow");

	return 0;
}

MODULE_FUNCTION_DEF(draw_translate)
{
	float x = (float) luaL_checknumber(L, 1);
	float y = (float) luaL_checknumber(L, 2);

	gDraw.translate(x, y);

	return 0;
}

MODULE_FUNCTION_DEF(draw_rotate)
{
	float r = (float) luaL_checknumber(L, 1);

	gDraw.rotate(r);

	return 0;
}

MODULE_FUNCTION_DEF(draw_scale)
{
	float sx = (float) luaL_checknumber(L, 1);
	float sy = (float) luaL_checknumber(L, 2);

	gDraw.scale(sx, sy);

	return 0;
}

MODULE_FUNCTION_DEF(draw_pushcliprect)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	float w = (float)luaL_checknumber(L, 3);
	float h = (float)luaL_checknumber(L, 4);

	if (!gDraw.pushClipRect(x, y, w, h))
	{
		return luaL_error(L, "Draw clip stack overflow");
	}

	return 0;
}

MODULE_FUNCTION_DEF(draw_pushclipplane)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	float nx = (float)luaL_checknumber(L, 3);
	float ny = (float)luaL_checknumber(L, 4);

	if (!gDraw.pushClipPlane(x, y, nx, ny))
	{
		return luaL_error(L, "Draw clip stack overflow");
	}

	return 0;
}

MODULE_FUNCTION_DEF(draw_popclip)
{
	if (!gDraw.popClip())
		return luaL_error(L, "Draw clip stack underflow");

	return 0;
}

static const luaL_Reg drawlib[] = {
	{"color", draw_color},
	{"rect", draw_rect},
	{"sprite", draw_sprite},
	{"quad", draw_quad},
	{"size", draw_size},
	{"layer", draw_layer},
	{"push", draw_push},
	{"pop", draw_pop},
	{"translate", draw_translate},
	{"rotate", draw_rotate},
	{"scale", draw_scale},
	{"pushcliprect", draw_pushcliprect},
	{"pushclipplane", draw_pushclipplane},
	{"popclip", draw_popclip},
	{nullptr, nullptr}
};

void Arcade::openLuaDrawModule(lua_State *L)
{
	luaL_register(L, "_r", drawlib);
	gDraw.init();
}

void Arcade::closeLuaDrawModule()
{
	gDraw.shutdown();
}