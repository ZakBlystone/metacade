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
asset.cpp:
===============================================================================
*/

#include "engine_private.h"

CAssetRef::CAssetRef()
{

}

CAssetRef::CAssetRef(class CPackage* package, IAsset* asset)
{
	if ( package == nullptr || asset == nullptr ) return;
	
	_package = package->getPackageID();
	_asset = asset->getUniqueID();
	_type = asset->getType();
}

EAssetType CAssetRef::getType() const
{
	return _type;
}

IAsset* CAssetRef::get(IRuntime* runtime) const
{
	CPackage* pkg = (CPackage*) getPackage(runtime);
	if ( pkg == nullptr ) return nullptr;

	return pkg->getAssetMap()->findAssetByID(_asset).get();
}

IPackage* CAssetRef::getPackage(IRuntime* runtime) const
{
	if ( runtime == nullptr ) return nullptr;

	return runtime->getPackageManager()->getPackageByID(_package);
}

CGUID CAssetRef::getAssetID() const
{
	return _asset;
}

CGUID CAssetRef::getPackageID() const
{
	return _package;
}