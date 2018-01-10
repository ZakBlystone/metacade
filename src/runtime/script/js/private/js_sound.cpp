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
js_sound.cpp: javascript sound interface
===============================================================================
*/

#include "js_private.h"

static v8::Global<v8::ObjectTemplate> gSoundTemplate;


Arcade::CSoundInterface::CSoundInterface()
	: _mixer(nullptr)
{

}

void Arcade::CSoundInterface::setMixer(ISoundMixer* mixer)
{
	_mixer = mixer;
}

static void jsThrow(v8::Isolate* isolate, const char* str)
{
	isolate->ThrowException( 
		v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal)
		.ToLocalChecked()
	);
}

static void playSound(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 1 )
	{
		jsThrow(isolate, "_s.play requires at least 1 argument");
		return;
	}

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( args.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	CAssetRef* sample = getJSUserdataValuePtr<CAssetRef>( args[0] );
	if ( sample == nullptr ) return;

	int32 targetChannel = EChannelID::CHANNEL_ANY;
	if ( args.Length() > 1 )
	{
		targetChannel = v8::Local<v8::Int32>::Cast( args[1] )->Value();
	}

	uint32 channel = sound->_mixer->playSound( *sample, targetChannel );
	args.GetReturnValue().Set( v8::Uint32::New( isolate, channel ) );
}

static void stopSound(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 1 )
	{
		jsThrow(isolate, "_s.stop requires at least 1 argument");
		return;
	}

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( args.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	int32 targetChannel = v8::Local<v8::Int32>::Cast( args[0] )->Value();

	sound->_mixer->stopSound(targetChannel);
}

static void setPitch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 2 )
	{
		jsThrow(isolate, "_s.pitch requires at least 2 arguments");
		return;
	}

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( args.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	int32 targetChannel = v8::Local<v8::Int32>::Cast( args[0] )->Value();
	float pitch = (float) v8::Local<v8::Number>::Cast( args[1] )->Value();

	sound->_mixer->setChannelPitch(targetChannel, pitch);
}

static void setVolume(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 2 )
	{
		jsThrow(isolate, "_s.volume requires at least 2 arguments");
		return;
	}

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( args.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	int32 targetChannel = v8::Local<v8::Int32>::Cast( args[0] )->Value();
	float volume = (float) v8::Local<v8::Number>::Cast( args[1] )->Value();

	sound->_mixer->setChannelVolume(targetChannel, volume);
}

static void setLooping(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	if ( args.Length() < 2 )
	{
		jsThrow(isolate, "_s.loop requires at least 2 arguments");
		return;
	}

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( args.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	int32 targetChannel = v8::Local<v8::Int32>::Cast( args[0] )->Value();
	bool loop = v8::Local<v8::Boolean>::Cast( args[1] )->Value();

	sound->_mixer->setChannelLooping(targetChannel, loop);
}

static void getMasterPitch(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Isolate* isolate = info.GetIsolate();
	v8::HandleScope scope(isolate);

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( info.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	info.GetReturnValue().Set( v8::Number::New( info.GetIsolate(), sound->_mixer->getMasterPitch() ) );
}

static void setMasterPitch(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Isolate* isolate = info.GetIsolate();
	v8::HandleScope scope(isolate);

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( info.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	sound->_mixer->setMasterPitch( (float) v8::Local<v8::Number>::Cast( value )->Value() );
}

static void getMasterVolume(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Isolate* isolate = info.GetIsolate();
	v8::HandleScope scope(isolate);

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( info.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	info.GetReturnValue().Set( v8::Number::New( info.GetIsolate(), sound->_mixer->getMasterVolume() ) );
}

static void setMasterVolume(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Isolate* isolate = info.GetIsolate();
	v8::HandleScope scope(isolate);

	CSoundInterface* sound = getJSUserdataPtr<CSoundInterface>( info.Holder() );
	if ( sound == nullptr || !sound->_mixer ) return;

	sound->_mixer->setMasterVolume( (float) v8::Local<v8::Number>::Cast( value )->Value() );
}

v8::Local<v8::ObjectTemplate> Arcade::getJSSoundWrapper(v8::Isolate* isolate)
{
	if ( !gSoundTemplate.IsEmpty() ) return gSoundTemplate.Get( isolate );

	v8::EscapableHandleScope handleScope( isolate );
	v8::Local<v8::ObjectTemplate> wrapper = v8::ObjectTemplate::New( isolate );
	wrapper->SetInternalFieldCount(1);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "play", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, playSound )
	);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "stop", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, stopSound )
	);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "pitch", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, setPitch )
	);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "volume", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, setVolume )
	);

	wrapper->Set(
		v8::String::NewFromUtf8( isolate, "loop", v8::NewStringType::kNormal ).ToLocalChecked(),
		v8::FunctionTemplate::New( isolate, setLooping )
	);

	wrapper->SetAccessor(
		v8::String::NewFromUtf8(isolate, "master_pitch", v8::NewStringType::kInternalized).ToLocalChecked(),
		getMasterPitch,
		setMasterPitch
	);

	wrapper->SetAccessor(
		v8::String::NewFromUtf8(isolate, "master_volume", v8::NewStringType::kInternalized).ToLocalChecked(),
		getMasterVolume,
		setMasterVolume
	);

	gSoundTemplate.Reset(isolate, wrapper);
	return handleScope.Escape( wrapper );
}

void Arcade::shutdownJSSound()
{
	gSoundTemplate.Reset();
}
