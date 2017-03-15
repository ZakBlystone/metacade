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

	virtual const char* getPackageName() const;
	virtual uint32 getNumAssets() const;
	virtual class IAsset* getAsset(uint32 index) const;

	virtual void loadAssets();
	virtual void releaseAssets();

	bool addAsset(shared_ptr<IAsset> asset);
	void removeAsset(IAsset* asset);

	bool save(IFileObject* file);
	bool load();

	bool hasPackageFlag(EPackageFlags flag);
	int32 getPackageFlags();

private:

	friend class CPackageManager;

	CPackage(CRuntimeObject* outer, IFileObject* file);

	IFileObject* _file;
	shared_ptr<CAssetMap> _map;

	vector<shared_ptr<CAssetMap::CAssetLoadHandle>> _loadHandles;
};

}