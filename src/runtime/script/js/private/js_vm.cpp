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
js_vm.cpp:
===============================================================================
*/

#include "js_private.h"

#include <iostream>
#include <windows.h>

class CArcadeAllocator : public v8::ArrayBuffer::Allocator
{

public:
	virtual void* Allocate(size_t length) override
	{
		log(LOG_MESSAGE, "Allocate-zeroed: %i", length);
		void* mem = gRuntime->getAllocator()->memrealloc(nullptr, length);
		memset( mem, 0, length );
		return mem;
	}

	virtual void* AllocateUninitialized(size_t length) override
	{
		log(LOG_MESSAGE, "Allocate: %i", length);
		return gRuntime->getAllocator()->memrealloc(nullptr, length);
	}

	virtual void Free(void* data, size_t length) override
	{
		log(LOG_MESSAGE, "Free: %i", length);
		gRuntime->getAllocator()->memfree(data);
	}
};

ELanguage CJavascriptVM::getLanguage()
{
	return LANG_JAVASCRIPT;
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

void CJavascriptVM::createGlobalTemplate()
{
	v8::Isolate::Scope isolate_scope(_isolate);
	v8::HandleScope handle_scope(_isolate);

	v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New( _isolate );
	
	global->Set(
		v8::String::NewFromUtf8( _isolate, "print", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( _isolate, printFunction )
	);

	v8::Local<v8::ObjectTemplate> template_keys = v8::ObjectTemplate::New( _isolate );

	for ( int32 i=0; i<EHIDKeyCode::KEY_MAX; ++i )
	{
		template_keys->Set(
			v8::String::NewFromUtf8( _isolate, *CHIDKeyCodes::getKeyName( (EHIDKeyCode) i ), v8::NewStringType::kNormal ).ToLocalChecked(),
			v8::Integer::New( _isolate, i )
		);
	}

	global->Set(
		v8::String::NewFromUtf8( _isolate, "keys", v8::NewStringType::kNormal ).ToLocalChecked(),
		template_keys
	);

	//_globalTemplate.Reset(_isolate, global);
	_globalTemplate.Set(_isolate, global);
}

bool CJavascriptVM::init()
{
	if ( _initialized ) return true;

	log(LOG_MESSAGE, "Initializing Javascript VM");

	//We need to look for this stuff where the .exe is
	char module_path[4096] = {0};
	GetModuleFileName(NULL, module_path, 4096);

	static const char* working_directory = "./";
	if ( !v8::V8::InitializeICUDefaultLocation(module_path) )
	{
		log(LOG_WARN, "ICU Disabled");
	}
	else
	{
		log(LOG_WARN, "ICU Enabled");
	}

	v8::V8::InitializeExternalStartupData(module_path);

	_platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(_platform.get());
	if ( !v8::V8::Initialize() )
	{
		log(LOG_ERROR, "Failed to initialize Javascript VM");
		return false;
	}

	_allocator =  new CArcadeAllocator(); //v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = _allocator;

	_isolate = v8::Isolate::New(create_params);

	createGlobalTemplate();

	_initialized = true;

	return true;
}

void CJavascriptVM::shutdown()
{
	if ( !_initialized ) return;
	_initialized = false;

	log(LOG_MESSAGE, "Shutdown Javascript VM");

	shutdownJSSound();
	shutdownJSDraw();
	shutdownJSAsset();

	_loadedClasses.clear();
	_isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	_platform.reset();
	delete _allocator;
}

bool CJavascriptVM::isRunning()
{
	return false;
}

weak_ptr<IVMClass> CJavascriptVM::loadGameVMClass(shared_ptr<CPackage> gamePackage)
{
	auto found = _loadedClasses.find(gamePackage->getPackageID());
	if (found != _loadedClasses.end())
	{
		if ( !(*found).second->loadFromPackage(gamePackage) )
		{
			return shared_ptr<CJavascriptVMClass>(nullptr);
		}

		return (*found).second;
	}
	else
	{
		shared_ptr<CJavascriptVMClass> newClass = makeShared<CJavascriptVMClass>(shared_from_this());
		_loadedClasses.insert(make_pair(gamePackage->getPackageID(), newClass));

		if (newClass->loadFromPackage(gamePackage))
		{
			return newClass;
		}
	}

	return shared_ptr<CJavascriptVMClass>(nullptr);
}


Arcade::CJavascriptVM::CJavascriptVM()
	: _initialized(false)
{

}

Arcade::CJavascriptVM::~CJavascriptVM()
{
	log(LOG_MESSAGE, "Destruct VM");
	shutdown();
	log(LOG_MESSAGE, "Finished Destruct VM");
}

bool Arcade::CJavascriptVM::includeGameScript()
{
	return false;
}

bool Arcade::CJavascriptVM::validateGameScript()
{
	return false;
}

v8::Local<v8::ObjectTemplate> CJavascriptVM::getGlobalTemplate()
{
	return _globalTemplate.Get(_isolate);
}

v8::Isolate* CJavascriptVM::getIsolate()
{
	return _isolate;
}