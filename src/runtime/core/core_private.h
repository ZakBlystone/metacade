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
core_private.h: Headers and macros private to this module
===============================================================================
*/

#include "metacade_private.h"
#include "core_public.h"

#pragma once

using namespace Arcade;

namespace Arcade
{

class CDefaultAllocator : public IAllocator
{
public:
	virtual void* memrealloc(void* pointer, uint32 size) override
	{
		return realloc(pointer, size);
	}

	virtual void memfree(void* pointer) override
	{
		free(pointer);
	}

	static CDefaultAllocator* get();
};

extern thread_local IAllocator* gAllocator;

template<typename T, typename... ArgT>
T *construct(ArgT&&... args)
{
	return new(zalloc(sizeof(T))) T(args...);
}

template<typename T>
void destroy(T* obj)
{
	if (obj == nullptr) return;
	obj->~T();
	zfree(obj);
}

template <typename T, typename... ArgT>
shared_ptr<T> makeShared(ArgT&&... args)
{
	return shared_ptr<T>
		(construct<T>(args...)
		, [](T* ptr) { destroy(ptr); });
}

extern void* zalloc(uint32 size);
extern void* zrealloc(void* pointer, uint32 size);
extern void zfree(const void* pointer);
extern void zfree(void* pointer);

}