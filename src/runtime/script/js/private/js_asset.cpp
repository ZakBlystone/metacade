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
js_asset.cpp: javascript asset interface
===============================================================================
*/

#include "js_private.h"

static v8::Global<v8::ObjectTemplate> gAssetTemplate;

static void assetRefGetName(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	CAssetRef* assetref = getJSUserdataPtr<CAssetRef>( info.Holder() );
	IAsset* locked = assetref ? assetref->get() : nullptr;
	info.GetReturnValue().Set(
		v8::String::NewFromUtf8( info.GetIsolate(), locked ? *locked->getName() : "<unnamed>", v8::NewStringType::kNormal).ToLocalChecked()
	);
}

static void assetRefGetType(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	CAssetRef* assetref = getJSUserdataPtr<CAssetRef>( info.Holder() );
	info.GetReturnValue().Set(
		v8::Uint32::New( info.GetIsolate(), assetref ? assetref->getType() : ASSET_NONE )
	);
}

v8::Local<v8::ObjectTemplate> Arcade::getJSAssetWrapper(v8::Isolate* isolate)
{
	if ( !gAssetTemplate.IsEmpty() ) return gAssetTemplate.Get( isolate );

	v8::EscapableHandleScope handleScope( isolate );
	v8::Local<v8::ObjectTemplate> wrapper = v8::ObjectTemplate::New( isolate );
	wrapper->SetInternalFieldCount(1);

	wrapper->SetAccessor(
		v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kInternalized).ToLocalChecked(),
		assetRefGetName
	);

	wrapper->SetAccessor(
		v8::String::NewFromUtf8(isolate, "type", v8::NewStringType::kInternalized).ToLocalChecked(),
		assetRefGetType
	);

	gAssetTemplate.Reset(isolate, wrapper);
	return handleScope.Escape( wrapper );
}

void Arcade::shutdownJSAsset()
{
	gAssetTemplate.Reset();
}
