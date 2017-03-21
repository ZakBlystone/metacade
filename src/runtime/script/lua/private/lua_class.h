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

class CLuaVMClass : public IVMClass, public enable_shared_from_this<CLuaVMClass>
{
public:
	CLuaVMClass(shared_ptr<class CLuaVM> host);
	virtual ~CLuaVMClass();

	virtual bool reload() override;
	virtual void buildAssets(CPackageBuilder* builder) override;

	virtual class IVMHost* getHost() override;
	virtual shared_ptr<CMetaData> getMetaData() override;
	virtual shared_ptr<IVMInstance> createVMInstance() override;

	bool pushLuaFunction(string functionName) const;
	bool loadFromFile(string filename);
	bool loadFromAsset(const CCodeAsset* asset);

	shared_ptr<CLuaVM> getLuaHost() const
	{
		return _host;
	}

private:

	static int metaFunctionCreate(lua_State *L);
	static int metaTextureCreate(lua_State *L);
	static int metaKeyvalCreate(lua_State *L);

	void createMetaTable(const char* name, lua_CFunction target);

	friend class CLuaVM;
	friend class CLuaVMInstance;

	shared_ptr<CMetaData> _metaData;

	string _lastLoadFile;
	shared_ptr<CLuaVM> _host;
	map<string, shared_ptr<LuaVMReference>> _functions;

	map<CString, CString> _textureLoadArgs;
	//map<CString, CString> _soundLoadArgs;
};

}