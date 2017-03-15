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
packagemanager.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

/*
class IPackage
{
public:
	virtual const char* getPackageName() const = 0;
	virtual uint32 getNumAssets() const = 0;
	virtual IAsset* getAsset(uint32 index) const = 0;

	virtual void loadAssets() = 0;
	virtual void releaseAssets() = 0;
};
*/

class CPackageManager : public IPackageManager, public CRuntimeObject
{
public:
	CPackageManager(CRuntimeObject* outer);

	virtual CPackage* createPackage() override;
	virtual void deletePackage(CPackage* package) override;

	virtual void setRootDirectory(const char* path) override;
	virtual const char* getRootDirectory() const override;

	virtual bool findAndPreloadPackages() override;
	virtual uint32 getNumPackages() const override;
	virtual CPackage* getPackage(uint32 index) const override;

private:
	string _rootPath;
	vector<shared_ptr<CPackage>> _references;
	
};

}