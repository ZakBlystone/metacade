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
assetmap.h: A collection of assets that can be saved and loaded
===============================================================================
*/

#pragma once

namespace Arcade
{

class CAssetMap : public enable_shared_from_this<CAssetMap>
{
public:
	CAssetMap();
	~CAssetMap();

	//An asset load handle is used to keep assets in memory until it is destructed
	//This allows multiple packages to hang on to shared assets
	class CAssetLoadHandle
	{
	public:
		~CAssetLoadHandle()
		{
			shared_ptr<CAssetMap> map = _map.lock();
			if ( map != nullptr )
				map->releaseAssets();
		}

	private:
		friend class CAssetMap;

		CAssetLoadHandle(shared_ptr<CAssetMap> map)
			: _map(map)
		{}

		weak_ptr<CAssetMap> _map;
	};

	#pragma pack(push, 1)

	//An asset locator tracks information about an asset in a file
	//i.e. size, type, offset in file, name
	struct CAssetLocator
	{
		CAssetLocator(IFileObject* obj = nullptr, IAsset* asset = nullptr)
			: _id(asset ? asset->getUniqueID() : CGUID())
			, _type(asset ? asset->getType() : EAssetType::ASSET_NONE)
			, _offset(obj ? obj->tell() : 0)
			, _size(0)
			, _isNamed(asset ? asset->isNamedAsset() : false)
		{}

		CGUID _id;
		EAssetType _type;
		uint32 _offset;
		uint32 _size;
		bool _isNamed;
	};

	#pragma pack(pop)

	void addDependency(shared_ptr<CAssetMap> other);
	void add(shared_ptr<IAsset> asset);
	void remove(IAsset* asset);

	uint32 getNumAssets() const;
	shared_ptr<IAsset> getAsset(uint32 index) const;

	shared_ptr<IAsset> findAssetByID(const CGUID& id) const;
	shared_ptr<IAsset> findAssetByName(const CString& name) const;

	bool save(IFileObject* file);
	bool load(IFileObject* file);

	bool hasLoadedAssets() const;

	shared_ptr<CAssetLoadHandle> loadAssets(IFileObject* file);

private:

	friend class CAssetLoadHandle;

	void releaseAssets();

	vector<shared_ptr<IAsset>> _assets;
	vector<CAssetLocator> _locators;
	map<CGUID, shared_ptr<IAsset>> _map;
	map<CGUID, shared_ptr<CAssetMap>> _dependencies;
	map<CString, shared_ptr<IAsset>> _nameMap;

	bool _assetsLoaded;
};

}