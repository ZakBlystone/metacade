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
	ASSET_ANY,
	ASSET_CODE,
	ASSET_TEXTURE,
	ASSET_SOUND,
};

class IAsset
{
public:
	virtual bool load(IFileObject* file) = 0;
	virtual bool save(IFileObject* file) = 0;
	virtual void release() = 0;
	virtual EAssetType getType() const = 0;
	virtual CGUID getUniqueID() const = 0;
	virtual bool isValidData() const = 0;
	virtual bool isLoaded() const = 0;
	virtual bool isNamedAsset() const = 0;
	virtual bool isInvalidated() const = 0;
	virtual CString getName() const = 0;
	virtual ~IAsset() = 0;

protected:
	friend class CPackageBuilder;
	friend class CAssetMap;

	virtual void setUniqueID(const CGUID &id) = 0;
	virtual void setLoaded(bool loaded) = 0;
	virtual void setName(const CString& name) = 0;
};

class INativeLoadableAsset
{
public:
	virtual bool loadNative() = 0;
	virtual bool unloadNative() = 0;
	virtual bool isNativeLoaded() const = 0;
};

template<EAssetType Type>
class CAsset : public IAsset
{
public:

	bool checkType() const { return _type == Type; }
	virtual EAssetType getType() const { return _type; }
	virtual CGUID getUniqueID() const { return _uniqueID; }
	virtual bool isLoaded() const { return _loaded; }
	virtual bool isNamedAsset() const { return !_name.empty(); }
	virtual CString getName() const { return _name; }
	virtual bool isInvalidated() const { return _invalidated; }

	static EAssetType getAssetType() { return Type; }
	void validate() { _invalidated = false; }

protected:
	friend class CPackageBuilder;
	friend class CAssetMap;

	CAsset() 
		: _type(Type)
		, _invalidated(false)
	{}

	void invalidate()
	{
		_invalidated = true;
	}

	void setUniqueID(const CGUID &id)
	{
		_uniqueID = id;
	}

	void setLoaded(bool loaded)
	{
		_loaded = loaded;
	}

	void setName(const CString& name)
	{
		_name = name;
	}

private:
	EAssetType _type;
	CGUID _uniqueID;
	CString _name;
	bool _loaded;
	bool _invalidated;
};

template<EAssetType Type, typename T>
class CNativeLoadableAsset : public CAsset<Type>, public INativeLoadableAsset
{
public:
	CNativeLoadableAsset()
		: CAsset<Type>()
		, _nativeLoaded(false)
		, _nativeData(nullptr)
	{}

	virtual bool loadNative()
	{
		if ( _nativeLoaded ) return true;
		if ( !isLoaded() ) return false;
		_nativeLoaded = handleLoadNative();
		return _nativeLoaded;
	}

	virtual bool unloadNative()
	{
		if ( !_nativeLoaded ) return true;
		if ( !handleUnloadNative() ) return false;
		_nativeLoaded = false;
		return true;
	}

	virtual void release()
	{
		unloadNative();
	}

	virtual bool isNativeLoaded() const
	{
		return _nativeLoaded;
	}

	T* getNativeData() const
	{
		return _nativeData;
	}

protected:

	virtual bool handleLoadNative() = 0;
	virtual bool handleUnloadNative() = 0;

	T* _nativeData;

private:
	bool _nativeLoaded;
};

class METACADE_API CAssetRef
{
public:
	CAssetRef();

	EAssetType getType() const;

	IAsset* get() const;
	IPackage* getPackage() const;

	CGUID getAssetID() const;
	CGUID getPackageID() const;

private:
	friend class CPackage;

	CAssetRef(class CPackage* package, IAsset* asset);

	CGUID _asset, _package;
	EAssetType _type;
};

template<typename T>
T* castAsset(const CAssetRef& ref)
{
	IAsset* asset = ref.get();
	if (!asset || !((T*)(asset))->checkType()) return nullptr;
	return (T*)asset;
}

}