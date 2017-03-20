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
lua_instance.h:
===============================================================================
*/

#include "engine_private.h"
#include "lua_reference.h"

#pragma once

namespace Arcade
{

class LuaVMInstance : public IVMInstance
{

public:
	LuaVMInstance(shared_ptr<class LuaVMClass> klass);
	virtual ~LuaVMInstance();

	virtual class IVMClass* getClass() override;
	virtual void setMachineEnvironment(IMachineEnvironment *env) override;
	virtual bool postCommand(const char** commandBuffer) override;
	virtual void postInputEvent(const class CInputEvent& input) override;
	virtual void precacheAssets(CPackageBuilder* builder) override;
	virtual void think(float seconds, float deltaSeconds) override;
	virtual void render(shared_ptr<class CElementRenderer> renderer) override;
	virtual void reset() override;

	virtual bool callFunction(CFunctionCall call) override;

	class LuaVM* getLuaHost() const;
	class LuaVMClass* getLuaClass() const;
	class LuaVMReference* getLuaObject() const;

	bool pcall(int nargs);

private:
	shared_ptr<class LuaVMClass> _klass;
	shared_ptr<LuaVMReference> _object;
};

}