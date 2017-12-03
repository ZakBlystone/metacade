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
runtimemanaged.cpp: Localizes runtime functionality to object
===============================================================================
*/

#include "engine_private.h"
#include <stdarg.h>

CRuntimeObject::CRuntimeObject(IRuntime* runtime)
	: _runtime(runtime)
{
}

CRuntimeObject::CRuntimeObject(CRuntimeObject* outer)
	: _runtime(outer != nullptr ? outer->_runtime : nullptr)
{
}

class IRuntime* CRuntimeObject::getRuntime() const
{
	return _runtime;
}