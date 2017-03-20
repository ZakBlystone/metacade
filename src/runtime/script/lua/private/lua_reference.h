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
lua_reference.h:
===============================================================================
*/

#pragma once

#include "engine_private.h"

#include "lua.hpp"

//VM stuff is private, we can convert these interfaces to make more sense now

namespace Arcade
{

class LuaVMReference
{
public:
	LuaVMReference(shared_ptr<class CLuaVM> host, int idx);
	~LuaVMReference();

	void push();

private:
	int _ref;
	shared_ptr<CLuaVM> _host;
};

}