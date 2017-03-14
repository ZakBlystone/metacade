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
	ASSET_NOTLOADED,

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
	virtual EAssetType getType() const = 0;
};

template<EAssetType Type>
class CAsset : public IAsset
{
public:

	bool checkType() const { return _type == Type; }
	virtual EAssetType getType() const { return _type; }

private:
	friend class CPackage;

	CAsset() : _type(Type) {}

	EAssetType _type;
};

template<typename T>
T* castAsset(IAsset* asset) { if (!asset || !((T*)(asset))->checkType()) return nullptr; return (T*)asset; }

}