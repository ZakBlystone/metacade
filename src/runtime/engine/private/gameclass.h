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
gameclass.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CGameClass : public IGameClass, public ITickable, public enable_shared_from_this<CGameClass>
{
public:
	virtual bool createInstance(IGameInstance** instance, void* userdata = nullptr) override;
	virtual void deleteInstance(IGameInstance* instance) override;

	virtual void tick(float deltatime);

	weak_ptr<CPackage> getPackage();

private:
	friend class CRuntime;

	CGameClass( weak_ptr<CPackage> package );

	bool init();
	void release();

	int32 _instanceCount;
	weak_ptr<CPackage> _package;
	weak_ptr<IVMClass> _vmKlass;
};

}