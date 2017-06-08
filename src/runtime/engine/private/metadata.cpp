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
metadata.cpp:
===============================================================================
*/

#include "engine_private.h"

uint32 CMetaData::getNumKeys() const
{
	return (uint32) _keys.size();
}

CString CMetaData::getKey(uint32 i) const
{
	return _keys[i];
}

CVariant CMetaData::getValue(uint32 i) const
{
	return getValue(_keys[i]);
}

void CMetaData::setKeyValuePair(const CString& key, const CVariant& value)
{
	CString lowkey = key.lower();

	auto existing = _keyvalues.find(lowkey);
	if ( existing != _keyvalues.end() )
	{
		(*existing).second = value;
		return;
	}

	_keyvalues.insert(make_pair(lowkey, value));
	_keys.push_back(lowkey);
}

bool CMetaData::save(IFileObject* file) const
{
	uint32 size = getNumKeys();
	if ( !file->write((uint32*)& size, sizeof(uint32)) ) return false;

	for ( uint32 i=0; i<size; ++i )
	{
		if ( !CFileUtil::writeString(file, getKey(i)) ) return false;
		if ( !CFileUtil::writeVariant(file, getValue(i)) ) return false;
	}

	return true;
}

bool CMetaData::load(IFileObject* file)
{
	_keyvalues.clear();
	_keys.clear();

	uint32 size;
	if ( !file->read((uint32*)& size, sizeof(uint32)) ) return false;

	for ( uint32 i=0; i<size; ++i )
	{
		CString key;
		CVariant value;
		if ( !CFileUtil::readString(file, key) ) return false;
		if ( !CFileUtil::readVariant(file, value) ) return false;
		setKeyValuePair(key, value);
	}

	return true;
}

CVariant CMetaData::getValue(const CString& key) const
{
	auto found = _keyvalues.find(key);
	if ( found == _keyvalues.end() ) return CVariant();

	return (*found).second;
}
