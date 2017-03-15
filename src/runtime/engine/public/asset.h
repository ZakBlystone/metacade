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
asset.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

enum EAssetType
{
	ASSET_NONE,
	ASSET_CODE,
	ASSET_TEXTURE,
	ASSET_SOUND,
};

class IAsset
{
public:
	virtual bool load(IFileObject* file) = 0;
	virtual bool save(IFileObject* file) = 0;
	virtual bool validate() const = 0;
	virtual void release() = 0;
	virtual EAssetType getType() const = 0;
	virtual CGUID getUniqueID() const = 0;
	virtual bool isLoaded() const = 0;

protected:
	friend class CPackage;
	friend class CAssetMap;

	virtual void setUniqueID(const CGUID &id) = 0;
	virtual void setLoaded(bool loaded) = 0;
};

template<EAssetType Type>
class CAsset : public IAsset, public CRuntimeObject
{
public:

	bool checkType() const { return _type == Type; }
	virtual EAssetType getType() const { return _type; }
	virtual CGUID getUniqueID() const { return _uniqueID; }
	virtual bool isLoaded() const { return _loaded; }

protected:
	friend class CPackage;
	friend class CAssetMap;

	CAsset(CRuntimeObject* object) 
		: CRuntimeObject(object)
		, _type(Type)
	{}

	void setUniqueID(const CGUID &id)
	{
		_uniqueID = id;
	}

	void setLoaded(bool loaded)
	{
		_loaded = loaded;
	}

private:
	EAssetType _type;
	CGUID _uniqueID;
	bool _loaded;
};

template<typename T>
T* castAsset(IAsset* asset) { if (!asset || !((T*)(asset))->checkType()) return nullptr; return (T*)asset; }

}