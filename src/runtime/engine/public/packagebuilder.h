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

class IAssetCompiler
{
public:
	virtual bool compile(IAsset* asset, class IMetaData* buildParameters) = 0;
};

class METACADE_API CPackageBuilder : public CRuntimeObject
{
public:

	~CPackageBuilder();

	template<typename T>
	T* addAsset()
	{
		T* newAsset = new T(this);
		newAsset->setUniqueID(CGUID::generate());
		addAsset(newAsset);
		return newAsset;
	}

	template<typename T>
	T* addNamedAsset(const CString& name)
	{
		T* newAsset = new T(this);
		newAsset->setName(name);
		newAsset->setUniqueID(CGUID::generate());
		addAsset(newAsset);
		return newAsset;
	}

	void setAssetCompiler(IAssetCompiler* compiler);
	IAssetCompiler* getAssetCompiler();

	void removeAsset(class IAsset* asset);

	IMetaData* getMetaData();

	class IAsset* findAssetByName(const CString& name);
	class IAsset* findAssetById(const CGUID& id);

	bool load();
	bool save();

private:
	friend class CPackageManager;

	void addAsset(class IAsset* asset);

	CPackageBuilder(class CPackage* package);

	IAssetCompiler* _compiler;

	class CPackage* _package;
};

}