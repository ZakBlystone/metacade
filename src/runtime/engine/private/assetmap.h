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
assetmap.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CAssetMap : public CRuntimeObject
{
public:
	CAssetMap(CRuntimeObject* outer);

	class CAssetLoadHandle
	{
	};

	struct CAssetLocator
	{
		CAssetLocator(IFileObject* obj = nullptr, IAsset* asset = nullptr)
			: _type(asset ? asset->getType() : EAssetType::ASSET_NOTLOADED)
			, _offset(obj ? obj->tell() : 0)
			, _size(0)
			, _id(asset ? asset->getUniqueID() : CGUID())
		{}

		EAssetType _type;
		uint32 _offset;
		uint32 _size;
		CGUID _id;
	};

	void addDependency(shared_ptr<CAssetMap> other);
	void add(shared_ptr<IAsset> asset);
	void remove(IAsset* asset);

	uint32 getNumAssets() const;
	shared_ptr<IAsset> getAsset(uint32 index) const;

	shared_ptr<IAsset> findAssetByID(const CGUID& id) const;

	bool save(IFileObject* file);
	bool load(IFileObject* file);

	shared_ptr<CAssetLoadHandle> loadAssets(IFileObject* file);

private:

	friend class CAssetLoadHandle;

	void releaseAssets();

	vector<shared_ptr<IAsset>> _assets;
	vector<CAssetLocator> _locators;
	map<CGUID, shared_ptr<IAsset>> _map;
	map<CGUID, shared_ptr<CAssetMap>> _dependencies;
};

}