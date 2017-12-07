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

void Arcade::testJavascript()
{
	std::cout << "TESTING JAVASCRIPT..." << std::endl;

	static const char* working_directory = "./";
	v8::V8::InitializeICUDefaultLocation(working_directory);
	v8::V8::InitializeExternalStartupData(working_directory);

	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	v8::Isolate* isolate = v8::Isolate::New(create_params);
	{
		v8::Isolate::Scope isolate_scope(isolate);
		// Create a stack-allocated handle scope.
		v8::HandleScope handle_scope(isolate);
		// Create a new context.
		v8::Local<v8::Context> context = v8::Context::New(isolate);
		// Enter the context for compiling and running the hello world script.
		v8::Context::Scope context_scope(context);
		// Create a string containing the JavaScript source code.
		v8::Local<v8::String> source =
			v8::String::NewFromUtf8(isolate, "'Hello' + ', World!'",
									v8::NewStringType::kNormal)
			.ToLocalChecked();
		// Compile the source code.
		v8::Local<v8::Script> script =
			v8::Script::Compile(context, source).ToLocalChecked();
		// Run the script to get the result.
		v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
		// Convert the result to an UTF8 string and print it.
		v8::String::Utf8Value utf8(isolate, result);
		std::cout << *utf8 << std::endl;
	}

	isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	delete create_params.array_buffer_allocator;
}

ELanguage CJavascriptVM::getLanguage()
{
	return LANG_JAVASCRIPT;
}

bool CJavascriptVM::init()
{
	log(LOG_MESSAGE, "Initializing Javascript VM");

	static const char* working_directory = "./";
	if ( !v8::V8::InitializeICUDefaultLocation(working_directory) )
	{
		log(LOG_WARN, "ICU Disabled");
	}
	else
	{
		log(LOG_WARN, "ICU Enabled");
	}

	v8::V8::InitializeExternalStartupData(working_directory);

	_platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(_platform.get());
	if ( !v8::V8::Initialize() )
	{
		log(LOG_ERROR, "Failed to initialize Javascript VM");
		return false;
	}

	_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = _allocator;

	_isolate = v8::Isolate::New(create_params);

	return true;
}

void CJavascriptVM::shutdown()
{
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

bool Arcade::CJavascriptVM::includeGameScript()
{
	return false;
}

bool Arcade::CJavascriptVM::validateGameScript()
{
	return false;
}

v8::Isolate* CJavascriptVM::getIsolate()
{
	return _isolate;
}
