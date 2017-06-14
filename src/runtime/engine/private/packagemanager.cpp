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
packagemanager.cpp:
===============================================================================
*/

#include "engine_private.h"

CPackageManager::CPackageManager(CRuntimeObject* outer) 
	: CRuntimeObject(outer)
	, _rootPath(".")
{
}

Arcade::CPackageManager::~CPackageManager()
{
	std::cout << "Destruct PACKAGE MANAGER" << std::endl;
}

CPackageBuilder* CPackageManager::createPackageBuilder(const CString& packageName)
{
	CString path = getRootDirectory() + "/" + packageName + ".mpkg";

	class CEnablePackageBuilder : public CPackageBuilder
	{
	public:
		CEnablePackageBuilder(class CPackage* package)
			: CPackageBuilder(package) {}
	};

	shared_ptr<CEnablePackageBuilder> builder = makeShared<CEnablePackageBuilder>( new CPackage(this, path) );
	_builders.push_back(builder);
	return builder.get();
}

void CPackageManager::deletePackageBuilder(CPackageBuilder* builder)
{
	for ( auto it = _builders.begin(); it != _builders.end(); ++it )
	{
		if ((*it).get() == builder)
		{
			_builders.erase(it);
			return;
		}
	}
}

void CPackageManager::setRootDirectory(const CString& path)
{
	_rootPath = path;
}

CString CPackageManager::getRootDirectory() const
{
	return _rootPath;
}

bool CPackageManager::findAndPreloadPackages()
{
	//_references.clear();

	CFileCollection files;
	if (!listFilesInDirectory(&files, *(_rootPath+"/"), "mpkg"))
	{
		return false;
	}

	for ( uint32 i=0; i<files.numFiles(); ++i )
	{
		CString filepath = files.getFile(i);
		shared_ptr<CPackage> found = findLoadedPackageFile(filepath);
		if ( found != nullptr ) 
		{
			found->load(true);
			continue;
		}

		log(LOG_MESSAGE, "Try load package %s", *filepath);

		class CEnablePackage : public CPackage
		{
		public:
			CEnablePackage(CRuntimeObject* outer, const CString& filepath)
				: CPackage(outer, filepath) {}
		};

		shared_ptr<CEnablePackage> newPackage = makeShared<CEnablePackage>(this, filepath);
		_references.push_back(newPackage);

		if ( !newPackage->load() )
		{
			log(LOG_WARN, "Failed to load package %s", *filepath);
		}
		else
		{
			log(LOG_MESSAGE, "Loaded package: %s [%s]", *filepath, *newPackage->getPackageName());
		}
	}

	return true;
}

uint32 CPackageManager::getNumPackages() const
{
	return (uint32) _references.size();
}

IPackage* CPackageManager::getPackage(uint32 index) const
{
	return _references[index].get();
}

IPackage* CPackageManager::getPackageByName(const CString& name) const
{
	for ( auto package : _references )
	{
		if ( package->getPackageName().lower() == name.lower() )
		{
			return package.get();
		}
	}
	return nullptr;
}

IPackage* Arcade::CPackageManager::getPackageByID(const CGUID& id) const
{
	for ( auto package : _references )
	{
		if ( package->getPackageID() == id )
		{
			return package.get();
		}
	}
	return nullptr;
}

shared_ptr<CPackage> CPackageManager::findLoadedPackageFile(const CString& filePath) const
{
	for ( auto package : _references )
	{
		if ( package->_filepath == filePath ) return package;
	}
	return nullptr;
}


shared_ptr<CPackage> CPackageManager::getSharedPackageByID(const CGUID& id) const
{
	for ( auto package : _references )
	{
		if ( package->getPackageID() == id )
		{
			return package;
		}
	}
	return nullptr;
}

void CPackageManager::unloadAllPackages()
{
	_references.clear();
}

