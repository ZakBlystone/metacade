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

template<typename T>
struct CRuntimeAllocator
{
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template<typename U>
    struct rebind {
        typedef CRuntimeAllocator<U> other;
    };


	inline explicit CRuntimeAllocator() {}
	inline ~CRuntimeAllocator() {}

	inline CRuntimeAllocator(CRuntimeAllocator const&) {}
	template<typename U> inline explicit CRuntimeAllocator(CRuntimeAllocator<U> const&) {}

    inline pointer address(reference r) { return &r; }
    inline const_pointer address(const_reference r) { return &r; }

	pointer allocate(std::size_t n, typename std::allocator<void>::const_pointer = 0)
	{
		return reinterpret_cast<pointer>( zalloc((uint32)n * sizeof(value_type)) );
	}

	void deallocate(pointer ptr, std::size_t)
	{
		zfree(ptr);
	}

    inline void construct(pointer p, const T& t) { new(p) T(t); }
    inline void destroy(pointer p) { p->~T(); }

    inline bool operator==(CRuntimeAllocator const&) { return true; }
    inline bool operator!=(CRuntimeAllocator const& a) { return !operator==(a); }
};

}