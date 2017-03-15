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
package.cpp:
===============================================================================
*/

#include "engine_private.h"

CPackage::CPackage(CRuntimeObject* outer, IFileObject* file)
	: CRuntimeObject(outer)
	, _file(file)
	, _map(new CAssetMap(this))
{

}

CPackage::~CPackage()
{
	if ( _file )
	{
		closeFIle(_file);
		_file = nullptr;
	}
	delete _map;
}

int32 CPackage::getNumAssets() const
{
	return _map->getNumAssets();
}

IAsset* CPackage::getAsset(int32 index) const
{
	return _map->getAsset(index).get();
}

bool CPackage::save(IFileObject* file)
{
	return _map->save(file);
}

bool CPackage::load()
{
	return _map->load(_file);
}

const char* CPackage::getPackageName()
{
	return "";
}

bool CPackage::hasPackageFlag(EPackageFlags flag)
{
	return false;
}

int32 CPackage::getPackageFlags()
{
	return 0;
}

void CPackage::removeAsset(IAsset* asset)
{
	_map->remove(asset);
}

void CPackage::loadAssets()
{

}

void CPackage::releaseAssets()
{

}

bool Arcade::CPackage::addAssetImplementation(class IAsset* asset)
{
	_map->add(shared_ptr<IAsset>(asset));
	return true;
}
