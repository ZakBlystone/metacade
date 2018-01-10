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
core.cpp:
===============================================================================
*/

#include "core_private.h"

Arcade::CDefaultAllocator* CDefaultAllocator::get()
{
	static CDefaultAllocator instance;
	return &instance;
}

thread_local IAllocator* Arcade::gAllocator = CDefaultAllocator::get();

void* Arcade::zalloc(uint32 size)
{
	return zrealloc(nullptr, size);
}

void* Arcade::zrealloc(void* pointer, uint32 size)
{
	size += sizeof(void**);
	uint8* mem = (uint8*) gAllocator->memrealloc(pointer, size);
	memcpy(mem, &gAllocator, sizeof(void**));

	void* val = *(void**) mem;

	return mem + sizeof(void*);
}

void Arcade::zfree(const void* pointer)
{
	zfree((void *)pointer);
}

void Arcade::zfree(void* pointer)
{
	uint8* mem = ((uint8*) pointer) - sizeof(void**);
	IAllocator* allocatedWith = *(IAllocator**)(mem);
	allocatedWith->memfree(mem);
}