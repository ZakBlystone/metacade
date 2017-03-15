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
assetmap.cpp:
===============================================================================
*/

#include "engine_private.h"

CAssetMap::CAssetMap(CRuntimeObject* outer)
	: CRuntimeObject(outer)
	, _assetsLoaded(false)
{

}

void CAssetMap::add(shared_ptr<IAsset> asset)
{
	_map.insert(make_pair(asset->getUniqueID(), asset));
	_assets.push_back(asset);
}

uint32 CAssetMap::getNumAssets() const
{
	return (uint32) _assets.size();
}

shared_ptr<IAsset> CAssetMap::getAsset(uint32 index) const
{
	return _assets[index];
}

shared_ptr<IAsset> CAssetMap::findAssetByID(const CGUID& id) const
{
	auto found = _map.find(id);
	if ( found != _map.end() ) return (*found).second;

	return nullptr;
}

void CAssetMap::addDependency(shared_ptr<CAssetMap> other)
{

}

void CAssetMap::remove(IAsset* asset)
{
	if ( asset == nullptr ) return;

	auto found = _map.find(asset->getUniqueID());
	if ( found != _map.end() )
	{
		_map.erase(found);
	}
}

bool CAssetMap::save(IFileObject* file)
{
	vector<CAssetLocator> tempLocators;

	uint32 numAssets = getNumAssets();
	if ( !file->write(&numAssets, sizeof(uint32)) ) return false;

	for ( shared_ptr<IAsset> asset : _assets )
	{
		CAssetLocator locator(file, asset.get());
		tempLocators.push_back(locator);
		if ( !file->write(&locator, sizeof(CAssetLocator)) ) return false;
	}

	for ( uint32 i=0; i<_assets.size(); ++i )
	{
		shared_ptr<IAsset> asset = _assets[i];
		CAssetLocator &locator = tempLocators[i];

		uint32 toLocator = locator._offset;
		locator._offset = file->tell();
		if ( !asset->save(file) )
		{
			return false;
		}
		locator._size = file->tell() - locator._offset;

		uint32 mark = file->tell();
		if ( !file->seek(toLocator) ) return false;
		if ( !file->write(&locator, sizeof(CAssetLocator)) ) return false;
		if ( !file->seek(mark) ) return false;
	}

	return true;
}

bool CAssetMap::load(IFileObject* file)
{
	_map.clear();
	_assets.clear();
	_locators.clear();

	if ( !file->seek(0) ) return false;

	uint32 numAssets;
	if ( !file->read(&numAssets, sizeof(uint32)) ) 
	{
		log(LOG_ERROR, "Failed to read asset count on package");
		return false;
	}

	for ( uint32 i=0; i<numAssets; ++i )
	{
		CAssetLocator locator;
		if ( !file->read(&locator, sizeof(CAssetLocator)) ) 
		{
			log(LOG_ERROR, "Failed to read asset locator on package");
			return false;
		}

		_locators.push_back(locator);

		shared_ptr<IAsset> asset = nullptr;
		switch(locator._type)
		{
		case Arcade::ASSET_NONE:
		break;
		case Arcade::ASSET_CODE:
			asset = make_shared<CCodeAsset>(this);
		break;
		case Arcade::ASSET_TEXTURE:
		break;
		case Arcade::ASSET_SOUND:
		break;
		default:
		break;
		}

		if ( asset == nullptr ) 
		{
			log(LOG_ERROR, "Failed to create asset on package");
			return false;
		}
		asset->setUniqueID(locator._id);
		_map.insert(make_pair(locator._id, asset));
		_assets.push_back(asset);
	}

	return true;
}

shared_ptr<CAssetMap::CAssetLoadHandle> CAssetMap::loadAssets(IFileObject* file)
{
	if ( _assetsLoaded ) return nullptr;

	for ( uint32 i=0; i<_locators.size(); ++i )
	{
		CAssetLocator &locator = _locators[i];

		if ( !file->seek(locator._offset) ) return nullptr;

		auto found = _map.find(locator._id);
		if ( found == _map.end() ) return nullptr;

		shared_ptr<IAsset> asset = (*found).second;
		if ( !asset->load(file) ) return nullptr;

		asset->setLoaded(true);
	}

	_assetsLoaded = true;

	return shared_ptr<CAssetLoadHandle>(new CAssetLoadHandle( shared_from_this() ) );
}

bool CAssetMap::hasLoadedAssets() const
{
	return _assetsLoaded;
}

void CAssetMap::releaseAssets()
{
	if ( !_assetsLoaded ) return;

	for ( shared_ptr<IAsset> asset : _assets )
	{
		asset->release();
		asset->setLoaded(false);
	}

	_assetsLoaded = false;
}
