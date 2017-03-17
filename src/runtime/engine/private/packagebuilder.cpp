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

CPackageBuilder::CPackageBuilder(class CPackage* package)
	: CRuntimeObject(package)
	, _package(package)
{
}

Arcade::CPackageBuilder::~CPackageBuilder()
{
	delete _package;
}

IMetaData* CPackageBuilder::getMetaData()
{
	return _package->getWritableMetaData();
}

bool CPackageBuilder::save(const CString& packageName)
{
	IPackageManager* manager = getRuntime()->getPackageManager();
	IFileObject* file = openFile(manager->getRootDirectory() + "/" + packageName + ".mpkg", FILE_WRITE);
	if ( file == nullptr ) return nullptr;

	bool saved = _package->save(file);

	closeFIle(file);
	return saved;
}

void CPackageBuilder::removeAsset(class IAsset* asset)
{
	_package->removeAsset(asset);
}

void CPackageBuilder::addAsset(IAsset* asset)
{
	_package->addAsset(shared_ptr<IAsset>(asset));
}
