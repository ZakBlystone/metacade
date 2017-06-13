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

struct CDrawData : public CRenderState
{
	CClipShape _viewClip;
	CColor _currentColor;
	CElementRenderer *_renderer;
	int32 _layer;

	inline bool valid()
	{
		return _renderer != nullptr;
	}

	inline CElementRenderer* getRenderer()
	{
		return _renderer;
	}
};

static CDrawData gData;

void Arcade::beginLuaDraw(lua_State *L, shared_ptr<CElementRenderer> renderer)
{
	gData._renderer = renderer.get();
	gData._layer = 0;
	gData._currentColor = CFloatColor(1.f, 1.f, 1.f, 1.f);
	gData._viewClip = renderer->getViewportClip();
	gData._material._blend = BLEND_NORMAL;
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

	static CFloatColor col;
	col.r = (float)luaL_checknumber(L, 1);
	col.g = (float)luaL_checknumber(L, 2);
	col.b = (float)luaL_checknumber(L, 3);
	col.a = (float)luaL_optnumber(L, 4, 1.f);

	gData._currentColor = col;

	if ( lua_gettop(L) > 4 )
	{
		gData._material._blend = (EBlendMode) luaL_checkinteger(L, 5);
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
	//uint32 t = (uint32)luaL_optnumber(L, 5, 0);
	CImageAsset* texture = opttexture(L, 5);
	float u0 = (float)luaL_optnumber(L, 6, 0);
	float v0 = (float)luaL_optnumber(L, 7, 0);
	float u1 = (float)luaL_optnumber(L, 8, 1);
	float v1 = (float)luaL_optnumber(L, 9, 1);

	float hw = w * .5f;
	float hh = h * .5f;
	float cx = x + hw;
	float cy = y + hh;

	int32 ccheck = 0;
	for (int32 i=0; i<gData._viewClip.getNumPlanes(); ++i )
	{
		const CHalfPlane& p = gData._viewClip.getHalfPlane(i);
		const float lcx = cx * p.x;
		const float lcy = cy * p.y;
		const float lw = hw * fabs(p.x);
		const float lh = hh * fabs(p.y);
		if ( lcx - lw + lcy - lh - p.z > 0 )
		{
			return 0;
		}
		else if ( lcx + lw + lcy + lh - p.z > 0 )
		{
			++ccheck;
		}
	}

	CRenderElement& el = gData.getRenderer()->addRenderElement();
	//gData._material._baseTexture = t;

	gData._material._baseTexture = texture != nullptr ? texture->getID() : 0;


	CVertex2D* verts = nullptr;
	if ( ccheck == 0 )
	{
		static CClipShape empty;
		verts = el.makeQuad2(empty, gData, gData._layer)._verts;
	}
	else
	{	
		verts = el.makeQuad2(gData._viewClip, gData, gData._layer)._verts;
	}

	verts[0]._position.x = x;
	verts[0]._position.y = y;
	verts[0]._texcoord.x = u0;
	verts[0]._texcoord.y = v0;
	verts[0]._color.irgba = gData._currentColor.irgba;

	verts[1]._position.x = x+w;
	verts[1]._position.y = y;
	verts[1]._texcoord.x = u1;
	verts[1]._texcoord.y = v0;
	verts[1]._color.irgba = gData._currentColor.irgba;

	verts[2]._position.x = x+w;
	verts[2]._position.y = y+h;
	verts[2]._texcoord.x = u1;
	verts[2]._texcoord.y = v1;
	verts[2]._color.irgba = gData._currentColor.irgba;

	verts[3]._position.x = x;
	verts[3]._position.y = y+h;
	verts[3]._texcoord.x = u0;
	verts[3]._texcoord.y = v1;
	verts[3]._color.irgba = gData._currentColor.irgba;

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
	//uint32 t = (uint32)luaL_optnumber(L, 6, 0);
	CImageAsset* texture = opttexture(L, 6);
	float u0 = (float)luaL_optnumber(L, 7, 0);
	float v0 = (float)luaL_optnumber(L, 8, 0);
	float u1 = (float)luaL_optnumber(L, 9, 1);
	float v1 = (float)luaL_optnumber(L, 10, 1);

	CMatrix3 xform;
	CMatrix3::identity(xform);

	gData._material._baseTexture = texture != nullptr ? texture->getID() : 0;

	//gData._material._baseTexture = t;

	xform.rotate(r);
	xform.translate(CVec2(x,y));

	CRenderQuad quad;
	quad.makeBox(CVec2(-w,-h), CVec2(w,h), gData._currentColor);
	quad.transform(xform);

	quad._verts[0]._texcoord.set(u0, v0);
	quad._verts[1]._texcoord.set(u1, v0);
	quad._verts[2]._texcoord.set(u1, v1);
	quad._verts[3]._texcoord.set(u0, v1);

	CRenderElement& el = gData.getRenderer()->addRenderElement();
	el.makeQuad(quad, gData._viewClip, gData, gData._layer);

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
		quad._verts[i]._color.irgba = gData._currentColor.irgba;
	}

	CImageAsset* texture = opttexture(L, 17);
	gData._material._baseTexture = texture != nullptr ? texture->getID() : 0;

	//gData._material._baseTexture = (uint32)luaL_optnumber(L, 17, 0);

	CRenderElement& el = gData.getRenderer()->addRenderElement();
	el.makeQuad(quad, gData._viewClip, gData, gData._layer);

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
	luaL_register(L, "_r", drawlib);
}