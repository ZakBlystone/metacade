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
runtimemanaged.cpp: Localizes runtime functionality to object
===============================================================================
*/

#include "engine_private.h"
#include <stdarg.h>

CRuntimeObject::CRuntimeObject(IRuntime* runtime)
	: _runtime(runtime)
{
}

CRuntimeObject::CRuntimeObject(CRuntimeObject* outer)
	: _runtime(outer != nullptr ? outer->_runtime : nullptr)
{
}


void CRuntimeObject::log(EMessageType type, const char* message, ...)
{
	CRuntime *runtime = (CRuntime* ) _runtime;
	va_list args;

	va_start(args, message);
	int32 len = _vscprintf(message, args) + 1;

	char* buffer = new char[len];
	vsprintf_s(buffer, len, message, args);

	ILogger* logger = runtime->getLogger();
	logger->log(buffer, type);

	delete [] buffer;
	va_end(args);
}

void* CRuntimeObject::zalloc(uint32 size)
{
	CRuntime *runtime = (CRuntime* ) _runtime;
	IAllocator* allocator = runtime->getAllocator();
	return allocator->memrealloc(nullptr, size);
}

void* CRuntimeObject::zrealloc(void* pointer, uint32 size)
{
	CRuntime *runtime = (CRuntime* ) _runtime;
	IAllocator* allocator = runtime->getAllocator();
	return allocator->memrealloc(pointer, size);
}

void CRuntimeObject::zfree(void* pointer)
{
	CRuntime *runtime = (CRuntime* ) _runtime;
	IAllocator* allocator = runtime->getAllocator();
	allocator->memfree(pointer);
}

void CRuntimeObject::zfree(const void* pointer)
{
	CRuntime *runtime = (CRuntime* ) _runtime;
	IAllocator* allocator = runtime->getAllocator();
	allocator->memfree((void *) pointer);
}

IFileObject* CRuntimeObject::openFile(const CString& name, EFileIOMode mode)
{
	CRuntime* runtime = (CRuntime* ) _runtime;
	IFileSystem* fs = runtime->getFilesystem();

	if ( fs == nullptr ) return nullptr;
	return fs->openFile(name, mode);
}

void CRuntimeObject::closeFIle(IFileObject* file)
{
	CRuntime* runtime = (CRuntime* ) _runtime;
	IFileSystem* fs = runtime->getFilesystem();

	if ( fs == nullptr ) return;
	fs->closeFile(file);
}

bool CRuntimeObject::listFilesInDirectory(IFileCollection* collection, const char* dir, const char* extFilter)
{
	CRuntime* runtime = (CRuntime* ) _runtime;
	IFileSystem* fs = runtime->getFilesystem();

	if ( fs == nullptr ) return false;
	return fs->listFilesInDirectory(collection, dir, extFilter);
}

class IRuntime* CRuntimeObject::getRuntime() const
{
	return _runtime;
}

CIndex CRuntimeObject::allocateImageIndex()
{
	CRuntime* runtime = (CRuntime* ) _runtime;
	return runtime->getImageIndexAllocator()->getIndex();
}

class IVMHost* Arcade::CRuntimeObject::getLuaVM()
{
	CRuntime* runtime = (CRuntime* ) _runtime;
	return runtime->getLuaVM();
}
