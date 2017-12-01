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
guid.cpp:
===============================================================================
*/

#include "core_private.h"

#ifdef WINDOWS
#include <objbase.h>
#endif

#ifdef LINUX
#include <uuid/uuid.h>
#endif

//Construct zero'd out GUID
Arcade::CGUID::CGUID()
	: _X(0)
	, _Y(0)
{

}

bool CGUID::operator==(const CGUID& other) const
{
	//XOR each 64-bit component of the two GUIDs, combine and check if 0
	return ((_X ^ other._X) | (_Y ^ other._Y)) == 0;
}

bool CGUID::operator!=(const CGUID& other) const
{
	//XOR each 64-bit component of the two GUIDs, combine and check if NOT 0
	return ((_X ^ other._X) | (_Y ^ other._Y)) != 0;
}

bool CGUID::operator<(const CGUID& other) const
{
	//Check to see if high 64-bit component of 'this' is less than 'other'
	if ( _X < other._X ) return true;

	//Otherwise, if the high bits of 'this' are greater than 'other', it's not less-than
	else if ( _X > other._X ) return false;

	//High bits are equal, check low bits
	else if ( _Y < other._Y ) return true;

	//Low bits were either greater-than or equal
	else return false;

	return false;
}

bool CGUID::isValid() const
{
	//Just check if non-zero
	return _X != 0 && _Y != 0;
}

CGUID CGUID::generate()
{
	CGUID out;

#ifdef WINDOWS
	//On Windows, create the native GUID* on casted CGUID
	CoCreateGuid((GUID *) &out);
#elif LINUX
	//On Linux, create a UUID on casted CGUID
	uuid_generate((unsigned char *) &out);
#else
	//No GUID implementation for this platform
	assert(false);
#endif

	return out;
}

void CGUID::reset()
{
	//Sets all parts to zero
	_X = _Y = 0;
}

const char* CGUID::tostring() const
{
	static char buf[64];

	sprintf_s(buf, "%08X-%08X-%08X-%08X", _A, _B, _C, _D);

	return buf;
}