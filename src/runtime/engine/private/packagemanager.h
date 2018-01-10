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

class CPackageManager : public IPackageManager
{
public:
	CPackageManager();
	~CPackageManager();

	virtual CPackageBuilder* createPackageBuilder(const CString& packageName = "unnamed");
	virtual void deletePackageBuilder(CPackageBuilder* builder);

	virtual void setRootDirectory(const CString& path) override;
	virtual CString getRootDirectory() const override;

	virtual bool findAndPreloadPackages() override;
	virtual uint32 getNumPackages() const override;
	virtual IPackage* getPackage(uint32 index) const override;
	virtual IPackage* getPackageByName(const CString& name) const override;
	virtual IPackage* getPackageByID(const CGUID& id) const override;

	virtual void unloadAllPackages();

private:
	friend class CPackageBuilder;
	friend class CRuntime;

	shared_ptr<CPackage> getSharedPackageByID(const CGUID& id) const;
	shared_ptr<CPackage> findLoadedPackageFile(const CString& filePath) const;

	CString _rootPath;
	vector<shared_ptr<CPackage>> _references;
	vector<shared_ptr<CPackageBuilder>> _builders;
};

}