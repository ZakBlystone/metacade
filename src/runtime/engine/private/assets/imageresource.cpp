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
imageresource.cpp:
===============================================================================
*/

#include "engine_private.h"

CImageAsset::CImageAsset(CRuntimeObject* outer)
	: CAsset(outer)
	, _index(nullptr)
{

}

CImageAsset::~CImageAsset()
{
	delete _index;
}

bool CImageAsset::load(IFileObject* file)
{
	if ( _index == nullptr ) 
		_index = new CIndex(allocateImageIndex());

	return true;
}

bool CImageAsset::save(IFileObject* file)
{
	return true;
}

bool CImageAsset::validate() const
{
	return true;
}

void CImageAsset::release()
{
	if ( _index != nullptr ) 
	{
		delete _index;
		_index = nullptr;
	}
}

int32 CImageAsset::getWidth() const
{
	return 0;
}

int32 CImageAsset::getHeight() const
{
	return 0;
}

int32 CImageAsset::getBytesPerPixel() const
{
	return 4;
}

EImagePixelFormat CImageAsset::getPixelFormat() const
{
	return EImagePixelFormat::PFM_RGBA8;
}

uint8* CImageAsset::getPixels() const
{
	return nullptr;
}

uint32 CImageAsset::getID() const
{
	return _index != nullptr ? _index->get() : 0;
}
