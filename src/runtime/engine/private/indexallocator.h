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
indexallocator.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CIndexAllocator : public CRuntimeObject, public enable_shared_from_this<CIndexAllocator>
{
public:
	class CIndexLock
	{
	public:
		CIndexLock()
			: _allocator()
			, _index(0)
		{
		
		}

		CIndexLock(weak_ptr<CIndexAllocator> weakallocator)
			: _allocator(weakallocator)
		{
			shared_ptr<CIndexAllocator> allocator = _allocator.lock();
			if ( allocator != nullptr ) allocator->lock(_index);
		}

		~CIndexLock()
		{
			auto allocator = _allocator.lock();
			if ( allocator != nullptr ) allocator->unlock(_index);
		}

		uint32 get() const
		{
			return _index;
		}

	private:
		uint32 _index;
		weak_ptr<CIndexAllocator> _allocator;
	};

	CIndexAllocator(CRuntimeObject* outer)
		: CRuntimeObject(outer)
	{}

	class CIndex getIndex();

private:
	friend class CIndexLock;

	void lock(uint32 &index);
	void unlock(uint32 index);

	map<uint32, bool> _indices;
};

class CIndex
{
public:
	uint32 get()
	{
		if ( _lock == nullptr ) return 0;
		return _lock->get();
	}

private:
	friend class CIndexAllocator;

	shared_ptr<CIndexAllocator::CIndexLock> _lock;
};

}