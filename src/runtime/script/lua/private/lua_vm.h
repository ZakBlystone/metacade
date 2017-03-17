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

//VM stuff is private, we can convert these interfaces to make more sense now

namespace Arcade
{

class LuaVMReference
{
public:
	LuaVMReference(shared_ptr<class LuaVM> host, int idx);
	~LuaVMReference();

	void push();

private:
	int _ref;
	shared_ptr<LuaVM> _host;
};

class LuaVMInstance : public IVMInstance
{

public:
	LuaVMInstance(shared_ptr<class LuaVMClass> klass);
	virtual ~LuaVMInstance();

	virtual class IVMClass* getClass() override;
	virtual void setMachineEnvironment(IMachineEnvironment *env) override;
	virtual bool postCommand(const char** commandBuffer) override;
	virtual void postInputEvent(const class CInputEvent& input) override;
	virtual void precacheAssets() override;
	virtual void think(float seconds, float deltaSeconds) override;
	virtual void render(shared_ptr<class CElementRenderer> renderer) override;
	virtual void reset() override;

	virtual bool callFunction(CFunctionCall call) override;

	shared_ptr<LuaVM> getLuaHost() const;

	shared_ptr<LuaVMClass> getLuaClass() const
	{
		return _klass;
	}

	shared_ptr<LuaVMReference> getLuaObject() const
	{
		return _object;
	}

	bool pcall(int nargs);

private:
	shared_ptr<LuaVMClass> _klass;
	shared_ptr<LuaVMReference> _object;
};

class LuaVMClass : public IVMClass, public enable_shared_from_this<LuaVMClass>
{
public:
	LuaVMClass(shared_ptr<class LuaVM> host);
	virtual ~LuaVMClass();

	virtual bool reload() override;
	virtual class CGameMetadata* getMetaData() override;
	virtual class IVMHost* getHost() override;
	virtual class IVMInstance* createVMInstance() override;
	virtual void shutdownVMInstance(IVMInstance* instance) override;

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

class LuaVM : public IVMHost, public enable_shared_from_this<LuaVM>
{
public:
	LuaVM();
	virtual ~LuaVM();
	virtual ELanguage getLanguage() override;
	virtual bool init() override;
	virtual void shutdown() override;
	virtual bool isRunning() override;
	virtual IVMClass* loadGameVMClass() override;
	virtual bool includeGameScript() override;
	virtual bool validateGameScript() override;

	bool pcall(int nargs);
	lua_State *getState();

	bool pushVariant(const CVariant& variant);

private:

	friend class LuaVMInstance;
	friend class LuaVMReference;

	lua_State *_L;
	unsigned int _memUsage;
	map<string, shared_ptr<LuaVMClass>> _loadedClasses;
};

extern shared_ptr<IVMHost> getLuaVM();

}