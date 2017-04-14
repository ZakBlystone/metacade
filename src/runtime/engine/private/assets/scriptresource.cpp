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
scriptresource.cpp:
===============================================================================
*/

#include "engine/engine_private.h"

Arcade::CCodeAsset::CCodeAsset(CRuntimeObject* outer) 
	: CAsset(outer)
	, _code(nullptr)
	, _codeLength(0)
{

}

bool CCodeAsset::load(IFileObject* file)
{
	if ( !file->read(&_codeLength, sizeof(uint32)) ) return false;
	
	_code = (char*) zrealloc(_code, _codeLength + 1);
	if ( _code == nullptr ) return false;

	_code[_codeLength] = 0;
	
	return file->read(_code, _codeLength);
}

bool CCodeAsset::save(IFileObject* file)
{
	if ( !file->write(&_codeLength, sizeof(uint32)) ) return false;
	if ( !file->write(_code, _codeLength) ) return false;
	return true;
}

bool CCodeAsset::validate() const
{
	if ( _code == nullptr ) return false;
	for ( uint32 i=0; i<_codeLength; ++i )
	{
		if ( (uint32)(_code[i]) > 127 ) return false;
	}
	return true;
}

const char* CCodeAsset::getCodeBuffer() const
{
	return _code;
}

uint32 CCodeAsset::getCodeLength() const
{
	return _codeLength;
}

void CCodeAsset::setCodeBuffer(const char* buffer, uint32 size)
{
	_code = (char*) zrealloc(_code, size);
	_codeLength = size;

	memcpy(_code, buffer, size);
}

void CCodeAsset::setCodeBuffer(const char* buffer)
{
	if ( buffer != nullptr ) setCodeBuffer(buffer, (uint32) strlen(buffer));
}

void CCodeAsset::release()
{
	if ( _code ) zfree(_code);
	_code = nullptr;
}