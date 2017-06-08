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

bool CFileUtil::writeVariant(IFileObject* file, const CVariant& value)
{
	EVariantType type = value.type();
	if ( !file->write(&type, sizeof(uint8)) ) return false;

	switch(type)
	{
		case VT_NONE:
		break;
		case VT_BOOLEAN:
			{
				bool writebool;
				if ( !value.get(writebool) || !file->write(&writebool, sizeof(bool)) ) return false;
			}
		break;
		case VT_UINT:
			{
				uint64 writeuint;
				if ( !value.get(writeuint) || !file->write(&writeuint, sizeof(uint64)) ) return false;
			}
		break;
		case VT_INT:
			{
				int64 writeint;
				if ( !value.get(writeint) || !file->write(&writeint, sizeof(int64)) ) return false;
			}
		break;
		case VT_DOUBLE:
			{
				double writedouble;
				if ( !value.get(writedouble) || !file->write(&writedouble, sizeof(double)) ) return false;
			}
		break;
		case VT_STRING:
			{
				CString writestr;
				if ( !value.get(writestr) || !CFileUtil::writeString(file, writestr) ) return false;
			}
		break;
		default:
		return false;
	}

	return true;
}

bool CFileUtil::readVariant(IFileObject* file, CVariant& value)
{
	EVariantType type = VT_NONE;
	if ( !file->read(&type, sizeof(uint8)) ) return false;

	switch(type)
	{
		case VT_NONE:
		break;
		case VT_BOOLEAN:
			{
				bool readbool;
				if ( !file->read(&readbool, sizeof(bool)) ) return false;
				value.set(readbool);
			}
		break;
		case VT_UINT:
			{
				uint64 readuint;
				if ( !file->read(&readuint, sizeof(uint64)) ) return false;
				value.set(readuint);
			}
		break;
		case VT_INT:
			{
				int64 readint;
				if ( !file->read(&readint, sizeof(int64)) ) return false;
				value.set(readint);
			}
		break;
		case VT_DOUBLE:
			{
				double readdouble;
				if ( !file->read(&readdouble, sizeof(double)) ) return false;
				value.set(readdouble);
			}
		break;
		case VT_STRING:
			{
				CString readstr;
				if ( !CFileUtil::readString(file, readstr) ) return false;
				value.set(readstr);
			}
		break;
		default:
		return false;
	}

	return true;
}
