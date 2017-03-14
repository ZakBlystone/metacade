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

CRuntimeObject::CRuntimeObject(CRuntime* runtime)
	: _runtime(runtime)
{
}

CRuntimeObject::CRuntimeObject(CRuntimeObject* outer)
	: _runtime(outer->_runtime)
{
}


void CRuntimeObject::log(EMessageType type, const char* message, ...)
{
	va_list args;

	va_start(args, message);
	int32 len = _vscprintf(message, args) + 1;

	char* buffer = new char[len];
	vsprintf_s(buffer, len, message, args);

	ILogger* logger = _runtime->getLogger();
	logger->log(buffer, type);

	delete [] buffer;
	va_end(args);
}

void* CRuntimeObject::alloc(unsigned int size)
{
	IAllocator* allocator = _runtime->getAllocator();
	return allocator->memrealloc(nullptr, size);
}

void* CRuntimeObject::realloc(void* pointer, unsigned int size)
{
	IAllocator* allocator = _runtime->getAllocator();
	return allocator->memrealloc(pointer, size);
}

void CRuntimeObject::free(void* pointer)
{
	IAllocator* allocator = _runtime->getAllocator();
	allocator->memfree(pointer);
}
