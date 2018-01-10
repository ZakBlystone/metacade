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
	: _package(package)
	, _compiler(nullptr)
{
}

Arcade::CPackageBuilder::~CPackageBuilder()
{
	delete _package;
}

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

	map->add(shared_ptr<IAsset>(asset, [this](IAsset* Del) {
		destroy(Del);
	}));
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

class IAsset* CPackageBuilder::constructAsset(EAssetType type)
{
	IAsset* newAsset = nullptr;
	switch(type)
	{
	case Arcade::ASSET_NONE: return nullptr;
	case Arcade::ASSET_ANY: return nullptr;
	case Arcade::ASSET_CODE: newAsset = construct<CCodeAsset>(); break;
	case Arcade::ASSET_TEXTURE: newAsset = construct<CImageAsset>(); break;
	case Arcade::ASSET_SOUND: newAsset = construct<CSoundAsset>(); break;
	default:
	break;
	}

	if ( newAsset != nullptr )
	{
		newAsset->setUniqueID(CGUID::generate());
	}

	return newAsset;
}