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
js_instance.cpp:
===============================================================================
*/

#include "js_private.h"

Arcade::CJavascriptVMInstance::CJavascriptVMInstance(weak_ptr<CJavascriptVMClass> klass) 
	: _klass(klass)
{

}

Arcade::IVMClass* Arcade::CJavascriptVMInstance::getClass()
{
	if (_klass.expired()) return nullptr;
	return (IVMClass*)_klass.lock().get();
}

void Arcade::CJavascriptVMInstance::setGameInstance(IGameInstance* gameInstance)
{

}

void Arcade::CJavascriptVMInstance::postInputEvent(const class CInputEvent& input)
{

}


void Arcade::CJavascriptVMInstance::init()
{
	if ( _klass.expired() ) return;
	shared_ptr<CJavascriptVMClass> klass = _klass.lock();

	v8::Isolate* isolate = klass->getIsolate();
	v8::Isolate::Scope isolate_scope(isolate);

	v8::HandleScope handle_scope(isolate);

	v8::Local<v8::Context> context = klass->getContext();
	v8::Context::Scope context_scope(context);

	v8::Local<v8::Object> global = context->Global();
	v8::MaybeLocal<v8::Value> xvalue = global->Get(context, v8::String::NewFromUtf8(isolate, "x"));

	if ( !xvalue.IsEmpty() && xvalue.ToLocalChecked()->IsNumber() )
	{
		v8::Local<v8::Number> xvaluenumber = xvalue.ToLocalChecked()->ToNumber();
		float x = xvaluenumber->Value();
		log(LOG_MESSAGE, "X = %f", x);
	}
}

void Arcade::CJavascriptVMInstance::think(float seconds, float deltaSeconds)
{

}

void Arcade::CJavascriptVMInstance::render(shared_ptr<class CElementRenderer> renderer)
{
	callFunction(CFunctionCall("draw"));
}

void Arcade::CJavascriptVMInstance::reset()
{

}

bool Arcade::CJavascriptVMInstance::callFunction(const CFunctionCall& call)
{
	if (_klass.expired()) return false;

	shared_ptr<CJavascriptVMClass> klass = _klass.lock();

	v8::Isolate* isolate = klass->getIsolate();
	v8::Isolate::Scope isolate_scope(isolate);

	v8::HandleScope handle_scope(isolate);

	v8::Local<v8::Context> context = klass->getContext();
	v8::Context::Scope context_scope(context);

	v8::Local<v8::Object> global = context->Global();
	v8::MaybeLocal<v8::Value> value = global->Get(context, v8::String::NewFromUtf8( isolate, call.getFunction().get() ));

	if ( !value.IsEmpty() )
	{
		v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast( value.ToLocalChecked() );
		v8::MaybeLocal<v8::Value> result = func->Call(context, global, 0, nullptr);

		return true;
	}
	else
	{
		return false;
	}

	return false;
}
