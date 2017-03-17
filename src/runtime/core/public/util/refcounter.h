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
refcounter.h: reference counting class to determine if outer is unique
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CReferenceCounter
{
public:
	CReferenceCounter()
		: _ref(new uint32(1))
	{}

	CReferenceCounter(const CReferenceCounter& other)
		: _ref(other._ref)
	{
		(*_ref)++;
	}

	~CReferenceCounter()
	{
		reset();
	}

	CReferenceCounter &operator=(const CReferenceCounter& other)
	{
		if ( &other == this ) return *this;
		reset();

		_ref = other._ref;
		(*_ref)++;

		return *this;
	}

	bool unique() const { return _ref == nullptr || (*_ref) <= 1; }
	uint32 count() const { return _ref == nullptr ? 0 : (*_ref); }

private:
	void reset()
	{
		if ( --(*_ref) == 0 )
		{
			delete _ref;
			_ref = nullptr;
		}
	}

	uint32 *_ref;
};

}