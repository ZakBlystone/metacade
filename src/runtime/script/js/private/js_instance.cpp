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


Arcade::CJavascriptVMInstance::~CJavascriptVMInstance()
{
	_context.Reset();
}

Arcade::IVMClass* Arcade::CJavascriptVMInstance::getClass()
{
	if (_klass.expired()) return nullptr;
	return (IVMClass*)_klass.lock().get();
}

void Arcade::CJavascriptVMInstance::setGameInstance(IGameInstance* gameInstance)
{
	_gameInstance = gameInstance;

	if ( _klass.expired() || _soundInterface.IsEmpty() ) return;

	shared_ptr<CJavascriptVMClass> klass = _klass.lock();

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( _soundInterface.Get( klass->getIsolate() ) );
	if ( sound == nullptr ) return;

	sound->setMixer( _gameInstance->getSoundMixer() );
}

void Arcade::CJavascriptVMInstance::postInputEvent(const class CInputEvent& input)
{
	bool success = false;
	float mouseX;
	float mouseY;

	CInputState previousState = _state;

	_state.applyEvent(input);
	_state.getMousePosition(mouseX, mouseY);

	switch (input.getEventType())
	{
	case INPUTEVENT_NONE:
	break;
	case INPUTEVENT_MOUSEPRESSED:
		success = !_state.getMouseIsFocused() || callFunction(CFunctionCall("onMousePressed"
			, mouseX
			, mouseY
			, (int32)input.getMouseButton()
			, _state.getMouseIsFocused()));
	break;
	case INPUTEVENT_MOUSERELEASED:
		success = !_state.getMouseIsFocused() || callFunction(CFunctionCall("onMouseReleased"
			, mouseX
			, mouseY
			, (int32)input.getMouseButton()
			, _state.getMouseIsFocused()));
	break;
	case INPUTEVENT_MOUSEMOVED:
		success = !_state.getMouseIsFocused() || callFunction(CFunctionCall("onMouseMoved"
			, input.getMouseX()
			, input.getMouseY()
			, input.getMouseDeltaX()
			, input.getMouseDeltaY()
			, _state.getMouseIsFocused()));
	break;
	case INPUTEVENT_KEYPRESSED:
	if (previousState.getKeyIsDown(input.getKeycode()))
	{
		success = callFunction(CFunctionCall("onKeyRepeat", (int32)input.getKeycode()));
	}
	else
	{
		success = callFunction(CFunctionCall("onKeyPressed", (int32)input.getKeycode()));
	}
	break;
	case INPUTEVENT_KEYRELEASED:
		success = callFunction(CFunctionCall("onKeyReleased", (int32)input.getKeycode()));
	break;
	case INPUTEVENT_FOCUSGAINED:
		success = callFunction(CFunctionCall("onFocusGained", (int32)input.getFocusElement()));
	break;
	case INPUTEVENT_FOCUSLOST:
		success = callFunction(CFunctionCall("onFocusLost", (int32)input.getFocusElement()));
	break;
	default:
	break;
	}
}


void Arcade::CJavascriptVMInstance::init()
{
	if ( _klass.expired() ) return;
	shared_ptr<CJavascriptVMClass> klass = _klass.lock();

	v8::Isolate* isolate = klass->getIsolate();
	v8::Isolate::Scope isolate_scope(isolate);

	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, klass->getVM()->getGlobalTemplate());

	v8::Context::Scope context_scope(context);
	v8::Local<v8::Object> global = context->Global();

	v8::Local<v8::ObjectTemplate> asset_template = getJSAssetWrapper(isolate);

	v8::Local<v8::Object> assetlist = v8::Object::New(isolate);
	v8::Local<v8::Object> drawInterface = newJSUserdata<CDrawInterface>( context, nullptr, getJSDrawWrapper(isolate) );
	v8::Local<v8::Object> soundInterface = newJSUserdata<CSoundInterface>( context, nullptr, getJSSoundWrapper(isolate) );

	auto package = klass->getPackage();
	for ( int32 i=0; i<package->getNumAssets(); ++i )
	{
		const CAssetRef& asset = package->getAsset(i);
		IAsset* locked = asset.get();
		if ( locked )
		{
			v8::Local<v8::Object> wrapped = newJSUserdata(context, &asset, asset_template);
			log(LOG_MESSAGE, "Write asset: %s", *locked->getName());
			assetlist->Set(
				v8::String::NewFromUtf8(isolate, *locked->getName(), v8::NewStringType::kInternalized)
					.ToLocalChecked(),
				wrapped
			);
		}
	}

	global->Set(
		context, 
		v8::String::NewFromUtf8( isolate, "assets", v8::NewStringType::kNormal ).ToLocalChecked(), 
		assetlist
	);

	global->Set(
		context, 
		v8::String::NewFromUtf8( isolate, "_r", v8::NewStringType::kNormal ).ToLocalChecked(), 
		drawInterface
	);

	global->Set(
		context, 
		v8::String::NewFromUtf8( isolate, "_s", v8::NewStringType::kNormal ).ToLocalChecked(), 
		soundInterface
	);
	log(LOG_MESSAGE, "Created asset list");


	_context.Reset(isolate, context);
	_drawInterface.Reset(isolate, drawInterface);
	_soundInterface.Reset(isolate, soundInterface);

	setGameInstance(_gameInstance);

	for ( int32 i=0; i<klass->getNumScripts(); ++i )
	{
		klass->getScript(i)->BindToCurrentContext()->Run(context);
	}
	
	v8::MaybeLocal<v8::Value> xvalue = global->Get(context, v8::String::NewFromUtf8(isolate, "x"));

	if ( !xvalue.IsEmpty() && xvalue.ToLocalChecked()->IsNumber() )
	{
		v8::Local<v8::Number> xvaluenumber = xvalue.ToLocalChecked()->ToNumber();
		float x = xvaluenumber->Value();
		log(LOG_MESSAGE, "X = %f", x);
	}

	callFunction(CFunctionCall("init"));
}

void Arcade::CJavascriptVMInstance::think(float seconds, float deltaSeconds)
{
	callFunction(CFunctionCall("think", seconds, deltaSeconds));
}

void Arcade::CJavascriptVMInstance::render(shared_ptr<class CElementRenderer> renderer)
{
	if ( _klass.expired() ) return;
	shared_ptr<CJavascriptVMClass> klass = _klass.lock();

	v8::HandleScope handle_scope( klass->getIsolate() );

	CDrawInterface* draw = getJSUserdataPtr<CDrawInterface>( _drawInterface.Get( klass->getIsolate() ) );
	if ( draw == nullptr ) return;

	draw->start( renderer.get() );

	callFunction(CFunctionCall("draw"));

	draw->end();
}

void Arcade::CJavascriptVMInstance::reset()
{
	callFunction(CFunctionCall("reset"));
}

bool Arcade::CJavascriptVMInstance::callFunction(const CFunctionCall& call)
{
	if (_klass.expired()) return false;

	shared_ptr<CJavascriptVMClass> klass = _klass.lock();

	v8::Isolate* isolate = klass->getIsolate();
	v8::Isolate::Scope isolate_scope(isolate);

	v8::HandleScope handle_scope(isolate);

	v8::Local<v8::Context> context = _context.Get(isolate);
	v8::Context::Scope context_scope(context);

	v8::Local<v8::Object> global = context->Global();
	v8::Local<v8::Value> value;

	if ( global->Get(context, v8::String::NewFromUtf8( isolate, call.getFunction().get() )).ToLocal(&value) )
	{
		if ( value->IsFunction() )
		{
			v8::Local<v8::Value>* values = new v8::Local<v8::Value>[ call.numArgs() ];
			for ( uint32 i=0; i<call.numArgs(); ++i )
			{
				CVariant arg = call.getArg(i);

				switch ( arg.type() )
				{
				case VT_NONE:
					values[i] = v8::Null(isolate);
				break;
				case VT_BOOLEAN:
				{
					bool bValue;
					values[i] = arg.get(bValue) ? v8::Local<v8::Value>::Cast(v8::Boolean::New(isolate, bValue)) : v8::Null(isolate);
				}
				break;
				case VT_UINT:
				{
					uint32 uiValue;
					values[i] = arg.get(uiValue) ? v8::Local<v8::Value>::Cast(v8::Uint32::NewFromUnsigned(isolate, uiValue)) : v8::Null(isolate);
				}
				break;
				case VT_INT:
				{
					int32 iValue;
					values[i] = arg.get(iValue) ? v8::Local<v8::Value>::Cast(v8::Int32::New(isolate, iValue)) : v8::Null(isolate);
				}
				break;
				case VT_DOUBLE:
				{
					double dValue;
					values[i] = arg.get(dValue) ? v8::Local<v8::Value>::Cast(v8::Number::New(isolate, dValue)) : v8::Null(isolate);
				}
				break;
				case VT_STRING:
				{
					CString sValue;
					values[i] = v8::Null(isolate);
					if ( arg.get(sValue) )
					{
						v8::Local<v8::String> str;
						if ( v8::String::NewFromUtf8( isolate, *sValue, v8::NewStringType::kNormal ).ToLocal(&str) )
						{
							values[i] = str;
						}
					}
				}
				break;
				default:
					values[i] = v8::Null(isolate);
				break;
				}
			}

			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);
			v8::MaybeLocal<v8::Value> result = func->Call(context, global, call.numArgs(), values);

			delete [] values;
			return true;
		}
	}
	else
	{
		return false;
	}

	return false;
}
