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
package.h:
===============================================================================
*/

#pragma once

#include "private/packagemanager.h"

namespace Arcade
{

class CPackage : public IPackage
{
public:
	CPackage(shared_ptr<CPackageManager> packageManager, IFileObject* file = nullptr);

	virtual int32 getNumAssets() override;
	virtual IAsset* getAsset(int32 index) override;

	virtual bool save() override;
	virtual bool load() override;

	virtual const char* getPackageName() override;
	virtual bool hasPackageFlag(EPackageFlags flag) override;
	virtual int32 getPackageFlags() override;

private:
	weak_ptr<CPackageManager> _manager;
	IFileObject* _file;
};

}