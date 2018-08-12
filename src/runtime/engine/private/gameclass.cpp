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
gameclass.cpp:
===============================================================================
*/

#include "engine_private.h"

CGameClass::CGameClass( weak_ptr<CPackage> package )
	: _package(package)
	, _instanceCount(0)
{

}

bool CGameClass::createInstance(IGameInstance** instance, void* userdata /*= nullptr*/)
{
	if ( instance == nullptr ) return false;

	if ( _instanceCount == 0 )
	{
		if ( !init() ) return false;
	}

	auto lockedKlass = _vmKlass.lock();
	if ( lockedKlass == nullptr ) return false;

	class CEnableGameInstance : public CGameInstance
	{
	public:
		CEnableGameInstance(weak_ptr<CGameClass> klass, shared_ptr<IVMInstance> vmInstance, void* userdata)
			: CGameInstance(klass, vmInstance, userdata) {}
	};

	*instance = construct<CEnableGameInstance>(shared_from_this(), lockedKlass->createVMInstance(), userdata);

	++_instanceCount;

	return true;
}

void CGameClass::deleteInstance(IGameInstance* instance)
{
	if ( instance == nullptr ) return;

	--_instanceCount;

	if ( _instanceCount == 0 )
	{
		release();
	}

	destroy(instance);
}

bool CGameClass::init()
{
	if ( _package.expired() ) return false;
	shared_ptr<CPackage> locked = _package.lock();

	if ( !locked->loadAssets() )
	{
		log(LOG_ERROR, "Failed to load assets from package");
		return false;
	}

	uint32 language;
	if ( !locked->getMetaData()->getValue("language").get(language) ) return false;
	if ( language > LANG_NUM ) return false;

	IVMHost* vm = gRuntime->getCodeVM( (ELanguage) language );

	if (vm == nullptr)
	{
		log(LOG_ERROR, "Language not supported in this build");
		return false;
	}

	_vmKlass = vm->loadGameVMClass(locked);
	if ( _vmKlass.expired() )
	{
		log(LOG_ERROR, "Failed to create game VM");
		return false;
	}

	return true;
}

void CGameClass::release()
{
	if ( _package.expired() ) return;
	shared_ptr<CPackage> locked = _package.lock();

	locked->releaseAssets();
}

weak_ptr<CPackage> CGameClass::getPackage()
{
	return _package;
}
