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
string.cpp: Replaces STL string, can be exported out of API
===============================================================================
*/

#include "core_private.h"

CString::CString()
	: _string(nullptr)
	, _length(0)
	, _refs(nullptr)
{}

CString::CString(uint32 length) 
	: _string(new char[length+1])
	, _length(length)
	, _refs(new uint32(1))
{

}

CString::CString(const CString& other)
{
	_refs = other._refs;
	_string = other._string;
	_length = other._length;

	if (_refs) (*_refs)++;
}

CString::CString(const char* str)
	: _refs(new uint32(1))
{
	_length = (uint32) strlen(str);
	_string = new char[_length+1];
	memcpy(_string, str, _length+1);
}

CString::~CString()
{
	reset();
}

uint32 CString::length() const
{
	return _length;
}

bool CString::empty() const
{
	return _length == 0;
}

CString CString::chopLeft(uint32 len) const
{
	if ( len >= length() ) return CString();
	CString output( length() - len );
	memcpy(output._string, _string + len, length() - len);
	output._string[length() - len] = 0;
	return output;
}

CString CString::chopRight(uint32 len) const
{
	if ( len >= length() ) return CString();
	CString output( length() - len );
	memcpy(output._string, _string, length() - len);
	output._string[length() - len] = 0;
	return output;
}

CString CString::sub(uint32 offset, uint32 len) const
{
	if ( offset >= length() ) return CString();
	if ( offset + len >= length() ) return chopLeft(offset);
	CString output( len );
	memcpy(output._string, _string + offset, len);
	output._string[len] = 0;
	return output;
}

CString CString::operator/(const CString& other) const
{
	if ( _length == 0 ) return CString("/") + other;

	if ( _string[_length-1] == '/' )
	{
		return (*this) + other;
	}
	return (*this) + "/" + other;
}

CString CString::operator/(const char* other) const
{
	return (*this) / CString(other);
}

CString CString::operator+(const CString &other) const
{
	CString output( length() + other.length() );
	memcpy(output._string, _string, length());
	memcpy(output._string+length(), other._string, other.length()+1);

	return output;
}

CString CString::operator+(const char *other) const
{
	return (*this) + CString(other);
}

CString &CString::operator=(const CString &other)
{
	if ( &other == this ) return *this;

	reset();

	_refs = other._refs;
	_string = other._string;
	_length = other._length;

	if (_refs) (*_refs)++;

	return *this;
}

bool CString::operator==(const CString& other) const
{
	if ( other._length != _length ) return false;

	return strcmp(get(), other.get()) == 0;
}


bool CString::operator!=(const CString& other) const
{
	return strcmp(get(), other.get()) != 0;
}

bool CString::operator<(const CString& other) const
{
	return strcmp(get(), other.get()) < 0;
}

const char *CString::operator*() const
{
	return get();
}

const char* CString::get() const
{
	if ( _string == nullptr ) return "";
	return _string;
}

void CString::reset()
{
	if ( _string != nullptr && --(*_refs) == 0 )
	{
		delete [] _string;
		delete _refs;

		_string = nullptr;
		_refs = nullptr;
	}
}

CString CString::lower() const
{
	CString out(*this);

	for ( uint32 i=0; i<out.length(); ++i )
	{
		char ch = out._string[i];
		if ( ch >= 'A' && ch <= 'Z' ) ch += 32;

		out._string[i] = ch;
	}
	return out;
}

CString CString::upper() const
{
	CString out(*this);

	for ( uint32 i=0; i<out.length(); ++i )
	{
		char ch = out._string[i];
		if ( ch >= 'a' && ch <= 'z' ) ch -= 32;

		out._string[i] = ch;
	}
	return out;
}
