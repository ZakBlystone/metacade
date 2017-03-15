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
packagebuilder.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CPackageBuilder : public CRuntimeObject
{
public:

	~CPackageBuilder();

	template<typename T>
	T* addAsset()
	{
		T* newAsset = new T(this);
		addAsset(newAsset);
		newAsset->setUniqueID(CGUID::generate());
		return newAsset;
	}

	template<typename T>
	T* addNamedAsset(const CString& name)
	{
		T* newAsset = new T(this);
		addAsset(newAsset);
		newAsset->setName(name);
		newAsset->setUniqueID(CGUID::generate());
		return newAsset;
	}

	void removeAsset(class IAsset* asset);
	bool save(const CString& packageName);

private:
	friend class CPackageManager;

	void addAsset(class IAsset* asset);

	CPackageBuilder(class CPackage* package);
	class CPackage* _package;
};

}