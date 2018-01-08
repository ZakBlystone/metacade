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
js_userdata.h: javascript userdata objects similar to Lua
===============================================================================
*/

#pragma once

namespace Arcade
{

typedef intptr_t DistinctClassID;

template<typename T>
class CClassIDGenerator
{
public:
	static DistinctClassID getDistinctClassID()
	{
		return (DistinctClassID) &getDistinctClassID;
	}
};

template<typename T>
struct CTypedObjectPack
{
	T _object;
	DistinctClassID _classid;

	using BaseType = typename std::decay<T>::type;

	CTypedObjectPack() {}
	CTypedObjectPack(T* object)
	{
		store(object);
	}

	void store( T* object )
	{
		_classid = CClassIDGenerator<BaseType>::getDistinctClassID();
		if ( object != nullptr ) memcpy( (void*)&_object, object, sizeof(T) );
	}

	bool check()
	{
		static const DistinctClassID match = CClassIDGenerator<BaseType>::getDistinctClassID();
		if ( _classid != match ) return false;
		return true;
	}
};

template<typename T>
static v8::Local<v8::Object> newJSUserdata( v8::Local<v8::Context> context, T* object, v8::Local<v8::ObjectTemplate> object_template )
{
	CTypedObjectPack<T> pack(object);

	v8::EscapableHandleScope handle_scope( context->GetIsolate() );
	v8::Local<v8::Object> wrapped = object_template->NewInstance(context).ToLocalChecked();
	v8::Local<v8::ArrayBuffer> copy = v8::ArrayBuffer::New( context->GetIsolate(), sizeof(CTypedObjectPack<T>) );

	memcpy( copy->GetContents().Data(), &pack, copy->ByteLength() );

	wrapped->SetInternalField(0, v8::Uint8Array::New( copy, 0, sizeof(CTypedObjectPack<T>) ) );
	return handle_scope.Escape(wrapped);
}

template<typename T>
static T* getJSUserdataPtr( v8::Local<v8::Object> object )
{
	v8::Local<v8::Uint8Array> buffer = v8::Local<v8::Uint8Array>::Cast(object->GetInternalField(0));
	if ( buffer->ByteLength() != sizeof(CTypedObjectPack<T>) ) return false;
	
	CTypedObjectPack<T>* pack = (CTypedObjectPack<T>*) buffer->Buffer()->GetContents().Data();
	return pack->check() ? &pack->_object : nullptr;
}

template<typename T>
static T* getJSUserdataValuePtr( v8::Local<v8::Value> value )
{
	return getJSUserdataPtr<T>( v8::Local<v8::Object>::Cast( value ) );
}

}