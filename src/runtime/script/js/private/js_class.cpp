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
js_class.cpp:
===============================================================================
*/

#include "js_private.h"

Arcade::CJavascriptVMClass::CJavascriptVMClass(weak_ptr<class CJavascriptVM> host) 
	: _host(host)
	, _metaData(makeShared<CMetaData>())
{

}

bool Arcade::CJavascriptVMClass::reload()
{
	return false;
}

class IVMHost* Arcade::CJavascriptVMClass::getHost()
{
	if (!_host.expired()) return _host.lock().get();
	return nullptr;
}

shared_ptr<class CMetaData> Arcade::CJavascriptVMClass::getMetaData()
{
	return _metaData;
}

shared_ptr<class IVMInstance> Arcade::CJavascriptVMClass::createVMInstance()
{
	return make_shared<CJavascriptVMInstance>(shared_from_this());
}

bool Arcade::CJavascriptVMClass::loadFromPackage(weak_ptr<CPackage> package)
{
	_package = package;

	shared_ptr<CPackage> locked = package.lock();
	if (locked == nullptr) return false;

	shared_ptr<CJavascriptVM> host = _host.lock();

	v8::Isolate* isolate = host->getIsolate();
	if (isolate == nullptr)
	{
		log(LOG_ERROR, "Couldn't get isolate");
		return false;
	}

	{
		v8::Isolate::Scope isolate_scope(isolate);
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Context> context = v8::Context::New(isolate);

		//context->SetSecurityToken( v8::Integer::New(isolate, 420) );

		v8::Context::Scope context_scope(context);

		for ( int32 i=0; i<locked->getNumAssets(); ++i )
		{
			const CAssetRef& asset = locked->getAsset(i);
			if ( asset.getType() == ASSET_CODE )
			{
				CCodeAsset* scriptSource = castAsset<CCodeAsset>(asset);
				if (scriptSource == nullptr)
				{
					log(LOG_ERROR, "Failed to load script asset '%s'", asset.getAssetID().tostring());
					return false;
				}

				CString origin_string = locked->getPackageName() + "." + scriptSource->getName();

				v8::Local<v8::String> source = v8::String::NewFromOneByte(
					isolate,
					(const uint8_t*)scriptSource->getCodeBuffer(),
					v8::NewStringType::kNormal,
					scriptSource->getCodeLength()).ToLocalChecked();

				v8::Local<v8::Script> script;
				v8::ScriptOrigin origin( v8::String::NewFromOneByte(isolate, (const uint8_t*)*origin_string, v8::NewStringType::kNormal).ToLocalChecked() );
				if ( !v8::Script::Compile(context, source, &origin).ToLocal<v8::Script>(&script) )
				{
					log(LOG_ERROR, "Failed to compile script '%s'", *origin_string);
					return false;
				}

				_scripts.push_back(
					v8::Persistent<v8::UnboundScript, v8::CopyablePersistentTraits<v8::UnboundScript>>(isolate, script->GetUnboundScript())
				);
			}
		}

		//log(LOG_MESSAGE, "OUTPUT: %s", *utf8);
		log(LOG_MESSAGE, "Compiled OK");
	}

	return true;
}

shared_ptr<CJavascriptVM> CJavascriptVMClass::getVM()
{
	return _host.lock();
}

v8::Isolate* Arcade::CJavascriptVMClass::getIsolate()
{
	shared_ptr<CJavascriptVM> host = _host.lock();

	v8::Isolate* isolate = host->getIsolate();
	if (isolate == nullptr)
	{
		log(LOG_ERROR, "Couldn't get isolate");
		return nullptr;
	}

	return isolate;
}

v8::Local<v8::UnboundScript> Arcade::CJavascriptVMClass::getScript(int32 index)
{
	shared_ptr<CJavascriptVM> host = _host.lock();

	v8::Isolate* isolate = host->getIsolate();
	if (isolate == nullptr)
	{
		log(LOG_ERROR, "Couldn't get isolate");
		return v8::Local<v8::UnboundScript>();
	}

	return _scripts[index].Get(isolate);
}

int32 Arcade::CJavascriptVMClass::getNumScripts()
{
	return _scripts.size();
}
