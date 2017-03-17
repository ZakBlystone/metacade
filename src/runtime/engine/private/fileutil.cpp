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
fileutil.cpp:
===============================================================================
*/

#include "engine_private.h"

bool CFileUtil::writeString(IFileObject* file, const CString &str)
{
	uint32 len = str.length();
	return file->write(&len, sizeof(uint32)) && file->write(*str, len);
}

bool CFileUtil::readString(IFileObject* file, CString& str)
{
	uint32 len;
	if ( !file->read(&len, sizeof(uint32)) ) return false;

	char* buffer = new char[len+1];
	if ( !file->read(buffer, len) )
	{
		delete [] buffer;
		return false;
	}
	buffer[len] = 0;
	str = CString(buffer);
	delete [] buffer;
	return true;
}
