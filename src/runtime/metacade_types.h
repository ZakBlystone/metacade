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
metacade_types.h: Common types used throughout the runtime
===============================================================================
*/

#pragma once

#ifdef WINDOWS
#if defined(RUNTIME_API_EXPORT)
#   define RUNTIME_API   __declspec(dllexport)
#else // outside DLL
#   define RUNTIME_API   __declspec(dllimport)
#endif
#else //Linux
	#define RUNTIME_API
#endif

enum EFileIOMode
{
	FILE_READ,
	FILE_WRITE,
};

enum EPointClassify
{
	PLANE_BEHIND = -1,
	PLANE_ON = 0,
	PLANE_INTERSECT = 0,
	PLANE_INFRONT = 1,
};

enum ELanguage
{
	LANG_DUMMY,
	LANG_PYTHON,
};

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;