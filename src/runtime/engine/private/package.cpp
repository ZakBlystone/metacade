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

CPackage::CPackage(shared_ptr<CPackageManager> packageManager, IFileObject* file)
	: _manager(packageManager)
	, _file(file)
{

}

int32 CPackage::getNumAssets()
{
	return 0;
}

IAsset* CPackage::getAsset(int32 index)
{
	return nullptr;
}

bool CPackage::save()
{
	return false;
}

bool CPackage::load()
{
	return false;
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
