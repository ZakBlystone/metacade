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
packagebuilder.cpp:
===============================================================================
*/

#include "engine_private.h"
#include "../script/lua/lua_private.h"

CPackageBuilder::CPackageBuilder(class CPackage* package)
	: CRuntimeObject(package)
	, _package(package)
	, _compiler(nullptr)
{
}

Arcade::CPackageBuilder::~CPackageBuilder()
{
	delete _package;
}

#if 0
bool Arcade::CPackageBuilder::setAndBuildMainScript(const CString& scriptPath)
{
	shared_ptr<CLuaVM> vm = make_shared<CLuaVM>(this); //getLuaVM();
	if ( vm == nullptr ) return false;

	IFileObject* scriptFile = openFile( getRuntime()->getPackageManager()->getRootDirectory() + "/" + scriptPath, FILE_READ);
	if (!scriptFile) return false;

	uint32 scriptSize = scriptFile->getSize();
	char* scriptBuffer = new char[scriptSize];
	if (!scriptFile->read(scriptBuffer, scriptSize)) return false;

	closeFIle(scriptFile);

	vm->init();

	CCodeAsset* mainScript = addNamedAsset<CCodeAsset>("main.lua");
	mainScript->setCodeBuffer(scriptBuffer, scriptSize);

	weak_ptr<IVMClass> klass = vm->loadGameVMClass(mainScript);
	shared_ptr<IVMClass> locked = klass.lock();
	if ( locked != nullptr )
	{
		locked->buildAssets(this);
	}

	vm->shutdown();

	return true;
}
#endif

IAssetCompiler* CPackageBuilder::getAssetCompiler()
{
	return _compiler;
}

void CPackageBuilder::setAssetCompiler(IAssetCompiler* compiler)
{
	_compiler = compiler;
}

IMetaData* CPackageBuilder::getMetaData()
{
	return _package->getWritableMetaData();
}

void CPackageBuilder::addAsset(IAsset* asset)
{
	CAssetMap* map = _package->getAssetMap();

	if ( asset->isNamedAsset() )
	{
		IAsset* existing = map->findAssetByName(asset->getName()).get();
		if ( existing != nullptr )
		{
			asset->setUniqueID(existing->getUniqueID());
			log(LOG_MESSAGE, "Overwrite existing named asset: %s", *asset->getName());
			map->remove(existing);
		}
	}

	map->add(shared_ptr<IAsset>(asset));
}

void CPackageBuilder::removeAsset(class IAsset* asset)
{
	_package->getAssetMap()->remove(asset);
}

class IAsset* CPackageBuilder::findAssetByName(const CString& name)
{
	return _package->getAssetMap()->findAssetByName(name).get();
}

class IAsset* CPackageBuilder::findAssetById(const CGUID& id)
{
	return _package->getAssetMap()->findAssetByID(id).get();
}

bool CPackageBuilder::load()
{
	if ( _package->load() )
	{
		return _package->loadAssets();
	}
	return false;
}

bool CPackageBuilder::save()
{
	return _package->save();
}