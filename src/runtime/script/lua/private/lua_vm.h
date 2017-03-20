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
lua_vm.h:
===============================================================================
*/

#pragma once

#include "engine_private.h"

#include "lua.hpp"
#include "lua_class.h"
#include "lua_reference.h"

//VM stuff is private, we can convert these interfaces to make more sense now

namespace Arcade
{

class CLuaVM : public IVMHost, public enable_shared_from_this<CLuaVM>
{
public:
	CLuaVM();
	virtual ~CLuaVM();
	virtual ELanguage getLanguage() override;
	virtual bool init() override;
	virtual void shutdown() override;
	virtual bool isRunning() override;
	virtual weak_ptr<IVMClass> loadGameVMClass(class CCodeAsset* codeAsset) override;
	virtual weak_ptr<IVMClass> loadGameVMClass() override;
	virtual bool includeGameScript() override;
	virtual bool validateGameScript() override;

	bool pcall(int nargs);
	lua_State *getState();

	bool pushVariant(const CVariant& variant);

private:

	friend class CLuaVMInstance;
	friend class LuaVMReference;

	lua_State *_L;
	unsigned int _memUsage;
	map<CString, shared_ptr<CLuaVMClass>> _loadedClasses;
};


}