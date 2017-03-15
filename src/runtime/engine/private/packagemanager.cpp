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

CPackage* CPackageManager::createPackage()
{
	shared_ptr<CPackage> newPackage = make_shared<CPackage>(this);

	_references.push_back(newPackage);

	return newPackage.get();
}

void CPackageManager::deletePackage(CPackage* package)
{
	//delete package;
}

void CPackageManager::setRootDirectory(const char* path)
{
	_rootPath = std::string(path);
}

const char*CPackageManager::getRootDirectory() const
{
	return _rootPath.c_str();
}

bool CPackageManager::findAndPreloadPackages()
{
	return false;
}

int32 CPackageManager::getNumPackages() const
{
	return 0;
}

CPackage* CPackageManager::getPackage(int32 index) const
{
	return nullptr;
}
