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
ipackagemanager.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

enum EPackageFlags
{
	PACKAGE_LOADED = 0x1,
	PACKAGE_READONLY = 0x2,
	PACKAGE_PRELOADED = 0x4,
};

class IPackage
{
public:
	virtual CString getPackageName() const = 0;
	virtual uint32 getNumAssets() const = 0;
	virtual class IAsset* getAsset(uint32 index) const = 0;
	virtual const IMetaData* getMetaData() const = 0;

	virtual bool loadAssets() = 0;
	virtual void releaseAssets() = 0;
};

class IPackageManager
{
public:
	virtual class CPackageBuilder* createPackageBuilder(const CString& packageName = "unnamed") = 0;
	virtual void deletePackageBuilder(class CPackageBuilder* builder) = 0;

	virtual void setRootDirectory(const CString& path) = 0;
	virtual CString getRootDirectory() const = 0;

	virtual bool findAndPreloadPackages() = 0;
	virtual uint32 getNumPackages() const = 0;
	virtual IPackage* getPackage(uint32 index) const = 0;

	virtual void unloadAllPackages() = 0;
};

}