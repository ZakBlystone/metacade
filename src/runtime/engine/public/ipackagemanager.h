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
};

class IPackageManager
{
public:
	virtual class CPackage* createPackage() = 0;
	virtual void deletePackage(class CPackage* package) = 0;

	virtual void setRootDirectory(const char* path) = 0;
	virtual const char* getRootDirectory() const = 0;

	virtual bool findAndPreloadPackages() = 0;
	virtual int32 getNumPackages() const = 0;
	virtual CPackage* getPackage(int32 index) const = 0;
};

}