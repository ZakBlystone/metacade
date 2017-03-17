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
	, _map(make_shared<CAssetMap>(this))
{

}

CPackage::~CPackage()
{
	if ( _file )
	{
		closeFIle(_file);
		_file = nullptr;
	}
}

uint32 CPackage::getNumAssets() const
{
	return _map->getNumAssets();
}

IAsset* CPackage::getAsset(uint32 index) const
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

CString CPackage::getPackageName() const
{
	return "";
}

bool CPackage::hasPackageFlag(EPackageFlags flag)
{
	return (getPackageFlags() & flag) != 0;
}

int32 CPackage::getPackageFlags()
{
	int32 flags = 0;
	if ( _map->hasLoadedAssets() ) flags |= EPackageFlags::PACKAGE_LOADED;

	return flags;
}

void CPackage::loadAssets()
{
	if ( !_map->hasLoadedAssets() )
	{
		_loadHandles.push_back(_map->loadAssets(_file));
	}
}

void CPackage::releaseAssets()
{
	_loadHandles.clear();
}

bool CPackage::addAsset(shared_ptr<IAsset> asset)
{
	_map->add(asset);
	return true;
}

void CPackage::removeAsset(IAsset* asset)
{
	_map->remove(asset);
}

const IMetaData* CPackage::getMetaData() const
{
	return _meta.get();
}

CMetaData* CPackage::getWritableMetaData()
{
	return _meta.get();
}
