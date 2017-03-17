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

Arcade::CGUID::CGUID()
	: X(0)
	, Y(0)
{

}

bool CGUID::operator==(const CGUID& other) const
{
	return ((X ^ other.X) | (Y ^ other.Y)) == 0;
}

bool CGUID::operator!=(const CGUID& other) const
{
	return ((X ^ other.X) | (Y ^ other.Y)) != 0;
}

bool CGUID::operator<(const CGUID& other) const
{
	if ( X < other.X ) return true;
	else if ( X > other.X ) return false;
	else if ( Y < other.Y ) return true;
	else return false;

	return false;
}

bool CGUID::isValid() const
{
	return X != 0 && Y != 0;
}

CGUID CGUID::generate()
{
	CGUID out;

#ifdef WINDOWS
	CoCreateGuid((GUID *) &out);
#elif LINUX
	uuid_generate((unsigned char *) &out);
#else
	//No GUID implementation for this platform
	assert(false);
#endif

	return out;
}

void CGUID::reset()
{
	X = Y = 0;
}

const char* CGUID::tostring() const
{
	static char buf[64];

	sprintf_s(buf, "%08X-%08X-%08X-%08X", A, B, C, D);

	return buf;
}