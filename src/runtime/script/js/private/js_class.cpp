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

	CCodeAsset* jsMain = castAsset<CCodeAsset>(locked->findAssetByName("main.code"));
	if (jsMain == nullptr)
	{
		log(LOG_ERROR, "Failed to load 'main.js'");
		return false;
	}

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

		v8::Context::Scope context_scope(context);

		v8::Local<v8::Object> global = context->Global();
		createGlobals(context, global);

		v8::Local<v8::String> source = v8::String::NewFromOneByte(
			isolate,
			(const uint8_t*)jsMain->getCodeBuffer(),
			v8::NewStringType::kNormal,
			jsMain->getCodeLength()).ToLocalChecked();

		v8::Local<v8::Script> script;
		if ( !v8::Script::Compile(context, source).ToLocal<v8::Script>(&script) )
		{
			log(LOG_ERROR, "Failed to compile script");
			return false;
		}

		//v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
		//v8::String::Utf8Value utf8(isolate, result);

		_context = v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>>(isolate, context);

		//log(LOG_MESSAGE, "OUTPUT: %s", *utf8);
	}

	return true;
}

static void printFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if ( args.Length() < 1 ) return;
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);
	v8::Local<v8::Value> arg = args[0];
	v8::String::Utf8Value value(isolate, arg);
	log(LOG_MESSAGE, "JS: %s", *value);
}

void Arcade::CJavascriptVMClass::createGlobals(v8::Local<v8::Context>& context, v8::Local<v8::Object>& global)
{
	global->Set(
		context,
		v8::String::NewFromUtf8( v8::Isolate::GetCurrent(), "print", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::Function::New( context, printFunction ).ToLocalChecked()
	);
}


v8::Local<v8::Context> Arcade::CJavascriptVMClass::getContext()
{
	shared_ptr<CJavascriptVM> host = _host.lock();

	v8::Isolate* isolate = host->getIsolate();
	if (isolate == nullptr)
	{
		log(LOG_ERROR, "Couldn't get isolate");
		return v8::Local<v8::Context>();
	}

	return _context.Get(isolate);
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