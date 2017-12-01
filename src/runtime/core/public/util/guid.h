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
guid.h: Globally unique identifier implementation
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CGUID
{
public:
	//Construct empty GUID
	CGUID();

	//Comparison operators
	bool operator == (const CGUID& other) const;
	bool operator != (const CGUID& other) const;
	bool operator < (const CGUID& other) const;

	//Check if GUID is valid (i.e. non-zero)
	bool isValid() const;

	//Generate a new GUID using the appropriate platform function
	static CGUID generate();

	//Set GUID to zero
	void reset();

	//Convert GUID to string using %08X-%08X-%08X-%08X pattern
	const char* tostring() const;

	//Union to hold 64-bit and 32-bit parts of the GUID
	union
	{
		struct
		{
			uint32 _A,_B,_C,_D;
		};
		struct
		{
			uint64 _X,_Y;
		};
	};
};

}