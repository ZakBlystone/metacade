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

#define PACKAGE_FORMAT_MAGIC 0x474B504D
#define PACKAGE_FORMAT_VERSION 3

//Version 2:
//	CVariants on CMetaData

//Version 3:
//  Texture flags + fixed memory alignment on CAssetLocator CGUID construction

#pragma pack(push, 1)

struct CPackHeader
{
	CPackHeader()
		: magic(PACKAGE_FORMAT_MAGIC)
		, version(PACKAGE_FORMAT_VERSION)
	{}

	uint32 magic;
	uint32 version;
	CGUID id;
	CSHA1 hash;
};

#pragma pack(pop)

CPackage::CPackage(const CString& filepath)
	: _filepath(filepath)
	, _map(makeShared<CAssetMap>())
	, _meta(makeShared<CMetaData>())
{
}

CPackage::~CPackage()
{
	std::cout << "DESTRUCT PACKAGE: " << *_filepath << std::endl;
}

uint32 CPackage::getNumAssets() const
{
	return _map->getNumAssets();
}

CAssetRef CPackage::getAsset(uint32 index) const
{
	return CAssetRef((CPackage*) this, _map->getAsset(index).get());
}

bool CPackage::save()
{
	CPackHeader header;
	CFileHandle file = openPackageFile(FILE_WRITE);

	if ( !file.isValid() )
	{
		log(LOG_ERROR, "Failed to open package for writing: %s", *_filepath);
		return false;
	}

	if ( !_uniqueID.isValid() )
	{
		_uniqueID = CGUID::generate();
	}
	header.id = _uniqueID;

	if ( !file->write(&header, sizeof(CPackHeader)) ) return false;

	if ( !_meta->save(file) ) return false;

	return _map->save(file);
}

bool CPackage::load(bool metaOnly /*= false*/)
{
	CPackHeader header;
	CFileHandle file = openPackageFile(FILE_READ);

	if ( !file.isValid() ) return false;

	if ( !file->read(&header, sizeof(CPackHeader)) ) return false;

	if ( header.magic != PACKAGE_FORMAT_MAGIC ) 
	{
		log(LOG_ERROR, "Error loading package, not a valid .mpkg file");
		return false;
	}

	if ( header.version != PACKAGE_FORMAT_VERSION )
	{
		log(LOG_ERROR, "Error loading package, incorrect version");
		return false;	
	}

	_uniqueID = header.id;

	if ( !_meta->load(file) ) return false;

	if ( metaOnly ) return true;

	return _map->load(file);
}

bool CPackage::exists()
{
	return openPackageFile(FILE_READ).isValid();
}

CString CPackage::getPackageName() const
{
	return _meta->getValue("name").toString();
}

CGUID CPackage::getPackageID() const
{
	return _uniqueID;
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

bool CPackage::loadAssets()
{
	CFileHandle file = openPackageFile(FILE_READ);

	if ( !file.isValid() ) 
	{
		log(LOG_ERROR, "Failed to open package: %s", *_filepath);
		return false;
	}

	if ( !_map->hasLoadedAssets() )
	{
		load(); //force refresh of asset list in case changed.

		shared_ptr<CAssetMap::CAssetLoadHandle> loaded = _map->loadAssets(file);
		if ( loaded == nullptr )
		{
			log(LOG_ERROR, "Failed to load package assets");
			return false;
		}
		_loadHandles.push_back(loaded);
	}

	return true;
}

void CPackage::releaseAssets()
{
	_loadHandles.clear();
}

const IMetaData* CPackage::getMetaData() const
{
	return _meta.get();
}

CMetaData* CPackage::getWritableMetaData()
{
	return _meta.get();
}

CAssetMap* CPackage::getAssetMap()
{
	return _map.get();
}

CFileHandle CPackage::openPackageFile(EFileIOMode mode)
{
	return CFileHandle(_filepath, mode);
}

CAssetRef CPackage::findAssetByID(const CGUID& id) const
{
	auto found = _map->findAssetByID(id);
	return CAssetRef((CPackage*) this, found.get());
}

CAssetRef CPackage::findAssetByName(const CString& name) const
{
	auto found = _map->findAssetByName(name);
	return CAssetRef((CPackage*) this, found.get());
}
