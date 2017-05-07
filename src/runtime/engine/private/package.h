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

namespace Arcade
{

class CPackage : public CRuntimeObject, public IPackage
{
public:
	~CPackage();

	virtual CString getPackageName() const;
	virtual CGUID getPackageID() const;
	virtual uint32 getNumAssets() const;
	virtual CAssetRef getAsset(uint32 index) const;
	virtual const IMetaData* getMetaData() const;

	virtual bool loadAssets();
	virtual void releaseAssets();

	virtual CAssetRef findAssetByID(const CGUID& id) const;
	virtual CAssetRef findAssetByName(const CString& name) const;

	bool save();
	bool load(bool metaOnly = false);
	bool exists();

	bool hasPackageFlag(EPackageFlags flag);
	int32 getPackageFlags();

	CMetaData* getWritableMetaData();
	CAssetMap* getAssetMap();

private:

	CFileHandle openPackageFile(EFileIOMode mode);

	friend class CPackageManager;

	CPackage(CRuntimeObject* outer, const CString& filepath);

	shared_ptr<CAssetMap> _map;
	shared_ptr<CMetaData> _meta;
	CGUID _uniqueID;

	CString _filepath;

	vector<shared_ptr<CAssetMap::CAssetLoadHandle>> _loadHandles;
};

}