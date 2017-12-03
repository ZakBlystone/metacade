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
indexallocator.cpp:
===============================================================================
*/

#include "engine_private.h"

CIndex CIndexAllocator::getIndex()
{
	CIndex out;
	out._lock = makeShared<CIndexAllocator::CIndexLock>(shared_from_this());
	return out;
}

void CIndexAllocator::lock(uint32 &index)
{
	index = 1;
	while ( _indices.find(index) != _indices.end() )
	{
		++index;
	}
	_indices.insert(make_pair(index, true));

	//log(LOG_MESSAGE, "---LOCK INDEX: %i", index);
}

void CIndexAllocator::unlock(uint32 index)
{
	auto found = _indices.find(index);
	if ( found != _indices.end() )
	{
		_indices.erase(found);
	}

	//log(LOG_MESSAGE, "---UNLOCK INDEX: %i", index);
}
