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
lua_class.h:
===============================================================================
*/

#pragma once

#include "lua.hpp"
#include "lua_reference.h"

//VM stuff is private, we can convert these interfaces to make more sense now

namespace Arcade
{

class LuaVMClass : public IVMClass, public enable_shared_from_this<LuaVMClass>
{
public:
	LuaVMClass(shared_ptr<class LuaVM> host);
	virtual ~LuaVMClass();

	virtual bool reload() override;
	virtual class CMetaData* getMetaData() override;
	virtual class IVMHost* getHost() override;
	virtual shared_ptr<IVMInstance> createVMInstance() override;

	bool pushLuaFunction(string functionName) const;
	bool loadFromFile(string filename);

	shared_ptr<LuaVM> getLuaHost() const
	{
		return _host;
	}

private:

	static int testMetaSet(lua_State *L);

	friend class LuaVM;
	friend class LuaVMInstance;

	string _lastLoadFile;
	shared_ptr<LuaVM> _host;
	map<string, shared_ptr<LuaVMReference>> _functions;
};

}