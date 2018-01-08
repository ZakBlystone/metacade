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
js_draw.cpp: javascript drawing interface
===============================================================================
*/

#include "js_private.h"
#include "render/render_private.h"

static v8::Global<v8::ObjectTemplate> gDrawTemplate;

Arcade::CDrawInterface::CDrawInterface() 
	: _renderer(nullptr)
	, _layer(0)
{

}


void CDrawInterface::start(CElementRenderer* renderer)
{
	_renderer = renderer;
	_layer = 0;
	_currentColor = CFloatColor(1.f, 1.f, 1.f, 1.f);
	_material._blend = BLEND_NORMAL;
}


void CDrawInterface::end()
{
	_renderer = nullptr;
}

void CDrawInterface::rect(const CRectData& data, const CAssetRef* asset)
{
	if ( !valid() ) return;

	CRenderElement& el = _renderer->addRenderElement();

	_material._baseTexture = resolveTextureID(asset);

	CRenderQuad* quad = &el.makeQuad2( _renderer->getViewportClip(), *this, _layer );
	CVertex2D* verts = quad->_verts;

	verts[0]._position._x = data._x;
	verts[0]._position._y = data._y;
	verts[0]._texcoord._x = data._u0;
	verts[0]._texcoord._y = data._v0;
	verts[0]._color._irgba = _currentColor._irgba;

	verts[1]._position._x = data._x+data._w;
	verts[1]._position._y = data._y;
	verts[1]._texcoord._x = data._u1;
	verts[1]._texcoord._y = data._v0;
	verts[1]._color._irgba = _currentColor._irgba;

	verts[2]._position._x = data._x+data._w;
	verts[2]._position._y = data._y+data._h;
	verts[2]._texcoord._x = data._u1;
	verts[2]._texcoord._y = data._v1;
	verts[2]._color._irgba = _currentColor._irgba;

	verts[3]._position._x = data._x;
	verts[3]._position._y = data._y+data._h;
	verts[3]._texcoord._x = data._u0;
	verts[3]._texcoord._y = data._v1;
	verts[3]._color._irgba = _currentColor._irgba;
}

void CDrawInterface::sprite(const CSpriteData& data, const CAssetRef* asset)
{
	if ( !valid() ) return;

	CMatrix3 xform;
	CMatrix3::identity(xform);

	_material._baseTexture = resolveTextureID(asset);

	xform.rotate( data._r );
	xform.translate( CVec2( data._x, data._y ) );

	CRenderQuad quad;
	quad.makeBox(
		CVec2( -data._w, -data._h ), 
		CVec2( data._w, data._h ),
		_currentColor
	);

	//quad.transform(gData.xformTop() * xform);
	quad.transform(xform);

	quad._verts[0]._texcoord.set( data._u0, data._v0 );
	quad._verts[1]._texcoord.set( data._u1, data._v0 );
	quad._verts[2]._texcoord.set( data._u1, data._v1 );
	quad._verts[3]._texcoord.set( data._u0, data._v1 );

	CRenderElement& el = _renderer->addRenderElement();
	el.makeQuad( quad, _renderer->getViewportClip(), *this, _layer );
}

CImageAsset* CDrawInterface::resolveTexture(const CAssetRef* asset)
{
	if ( asset == nullptr || asset->getType() != ASSET_TEXTURE ) return nullptr;
	return castAsset<CImageAsset>(*asset);
}

uint32 Arcade::CDrawInterface::resolveTextureID(const CAssetRef* asset)
{
	CImageAsset* texture = resolveTexture( asset );
	return texture ? texture->getID() : 0;
}

bool CDrawInterface::valid() const
{
	return _renderer != nullptr;
}

static void jsThrow(v8::Isolate* isolate, const char* str)
{
	isolate->ThrowException( 
		v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal)
		.ToLocalChecked()
	);
}

static void drawColor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 3 )
	{
		jsThrow(isolate, "_r.color requires at least 3 arguments");
		return;
	}

	CDrawInterface* draw = getJSUserdataPtr<CDrawInterface>( args.Holder() );
	if ( draw == nullptr ) return;

	static CFloatColor col;
	col.r = (float) v8::Local<v8::Number>::Cast(args[0])->Value();
	col.g = (float) v8::Local<v8::Number>::Cast(args[1])->Value();
	col.b = (float) v8::Local<v8::Number>::Cast(args[2])->Value();

	if ( args.Length() > 3 ) col.a = (float) v8::Local<v8::Number>::Cast(args[3])->Value();

	draw->_currentColor = col;

	if ( args.Length() > 4 ) draw->_material._blend = (EBlendMode) v8::Local<v8::Uint32>::Cast(args[4])->Value();
}

static void drawRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 4 )
	{
		jsThrow(isolate, "_r.rect requires at least 4 arguments");
		return;
	}

	CDrawInterface* draw = getJSUserdataPtr<CDrawInterface>( args.Holder() );
	if ( draw == nullptr ) return;

	CDrawInterface::CRectData rect;
	rect._x = (float) v8::Local<v8::Number>::Cast(args[0])->Value();
	rect._y = (float) v8::Local<v8::Number>::Cast(args[1])->Value();
	rect._w = (float) v8::Local<v8::Number>::Cast(args[2])->Value();
	rect._h = (float) v8::Local<v8::Number>::Cast(args[3])->Value();

	if ( args.Length() > 5 ) rect._u0 = (float) v8::Local<v8::Number>::Cast(args[5])->Value();
	if ( args.Length() > 6 ) rect._u0 = (float) v8::Local<v8::Number>::Cast(args[6])->Value();
	if ( args.Length() > 7 ) rect._u0 = (float) v8::Local<v8::Number>::Cast(args[7])->Value();
	if ( args.Length() > 8 ) rect._u0 = (float) v8::Local<v8::Number>::Cast(args[8])->Value();

	draw->rect( rect, ( args.Length() > 4 ) ? getJSUserdataValuePtr<CAssetRef>( args[4] ) : nullptr );
}

static void drawSprite(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 4 )
	{
		jsThrow(isolate, "_r.sprite requires at least 4 arguments");
		return;
	}

	CDrawInterface* draw = getJSUserdataPtr<CDrawInterface>( args.Holder() );
	if ( draw == nullptr ) return;

	CDrawInterface::CSpriteData sprite;
	sprite._x = (float) v8::Local<v8::Number>::Cast(args[0])->Value();
	sprite._y = (float) v8::Local<v8::Number>::Cast(args[1])->Value();
	sprite._w = (float) v8::Local<v8::Number>::Cast(args[2])->Value() * .5f;
	sprite._h = (float) v8::Local<v8::Number>::Cast(args[3])->Value() * .5f;

	if ( args.Length() > 4 ) sprite._r = (float) v8::Local<v8::Number>::Cast(args[4])->Value();

	if ( args.Length() > 6 ) sprite._u0 = (float) v8::Local<v8::Number>::Cast(args[6])->Value();
	if ( args.Length() > 7 ) sprite._u0 = (float) v8::Local<v8::Number>::Cast(args[7])->Value();
	if ( args.Length() > 8 ) sprite._u0 = (float) v8::Local<v8::Number>::Cast(args[8])->Value();
	if ( args.Length() > 9 ) sprite._u0 = (float) v8::Local<v8::Number>::Cast(args[9])->Value();

	draw->sprite( sprite, ( args.Length() > 5 ) ? getJSUserdataValuePtr<CAssetRef>( args[5] ) : nullptr );
}

v8::Local<v8::ObjectTemplate> Arcade::getJSDrawWrapper(v8::Isolate* isolate)
{
	if ( !gDrawTemplate.IsEmpty() ) return gDrawTemplate.Get( isolate );

	v8::EscapableHandleScope handleScope( isolate );
	v8::Local<v8::ObjectTemplate> wrapper = v8::ObjectTemplate::New( isolate );
	wrapper->SetInternalFieldCount(1);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "color", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, drawColor )
	);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "rect", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, drawRect )
	);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "sprite", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, drawSprite )
	);

	gDrawTemplate.Reset(isolate, wrapper);
	return handleScope.Escape( wrapper );
}

