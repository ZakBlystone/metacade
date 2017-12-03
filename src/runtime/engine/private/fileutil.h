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
fileutil.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

//TODO make a base class or something for reference counting
class CFileHandle
{
public:
	CFileHandle(const CString& filename, EFileIOMode mode);

	CFileHandle(const CFileHandle& other)
	{
		_ref = other._ref;
		_file = other._file;

		if ( _ref ) (*_ref)++;
	}

	bool isValid()
	{
		return _file != nullptr;
	}

	~CFileHandle() { reset(); }

	CFileHandle& operator=(const CFileHandle& other)
	{
		if ( &other == this ) return *this;
		
		reset();

		_ref = other._ref;
		_file = other._file;

		if ( _ref ) (*_ref)++;
		return *this;
	}

	void reset();

	operator IFileObject*()
	{
		return _file;
	}

	IFileObject* operator -> ()
	{
		return _file;
	}

private:
	IFileObject* _file;
	uint32* _ref;
};

class CFileUtil
{
public:
	static bool writeString(IFileObject* file, const CString &str);
	static bool readString(IFileObject* file, CString& str);

	static bool writeVariant(IFileObject* file, const CVariant& value);
	static bool readVariant(IFileObject* file, CVariant& value);
};

}