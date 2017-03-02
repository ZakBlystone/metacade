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
ifilesystem.h:
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

class IFileObject
{
public:
	virtual bool read(void* data, uint32 bytes) = 0;
	virtual bool write(void* data, uint32 bytes) = 0;
	virtual bool seek(uint32 offset) = 0;
	virtual uint32 tell() = 0;
	virtual uint32 getSize() = 0;
};

class IFileSystem
{
public:
	virtual IFileObject* openFile(const char* filename, EFileIOMode mode) = 0;
	virtual void closeFile(IFileObject* file) = 0;
	virtual const char** listFilesInDirectory(const char* dir, const char* extFilter = nullptr) = 0;
};

}