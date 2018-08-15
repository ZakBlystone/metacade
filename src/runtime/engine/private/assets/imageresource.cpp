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

CImageAsset::CImageAsset()
	: CNativeLoadableAsset()
	, _flags(0)
	, _index(nullptr)
	, _pixels(nullptr)
{

}

CImageAsset::~CImageAsset()
{
	release();
}

bool CImageAsset::load(IFileObject* file)
{
	if ( !file->read(&_format, sizeof(EImagePixelFormat)) ) return false;
	if ( !file->read(&_bpc, sizeof(uint8)) ) return false;
	if ( !file->read(&_width, sizeof(int32)) ) return false;
	if ( !file->read(&_height, sizeof(int32)) ) return false;
	if ( !file->read(&_flags, sizeof(uint32)) ) return false;

	if ( _index == nullptr ) 
		_index = construct<CIndex>(gRuntime->getImageIndexAllocator()->getIndex());

	if ( _pixels != nullptr )
		release();

	uint32 size = _bpc * _width * _height;
	_pixels = (uint8*) zalloc(size); //new uint8[size];

	if ( !file->read(_pixels, size) ) return false;

	return true;
}

bool CImageAsset::save(IFileObject* file)
{
	if ( !file->write(&_format, sizeof(EImagePixelFormat)) ) return false;
	if ( !file->write(&_bpc, sizeof(uint8)) ) return false;
	if ( !file->write(&_width, sizeof(int32)) ) return false;
	if ( !file->write(&_height, sizeof(int32)) ) return false;
	if ( !file->write(&_flags, sizeof(uint32)) ) return false;

	if ( _pixels == nullptr ) return false;

	if ( !file->write(_pixels, _bpc * _width * _height) ) return false;

	return true;
}

bool CImageAsset::isValidData() const
{
	return true;
}

void CImageAsset::release()
{
	CNativeLoadableAsset::release();

	if ( _index != nullptr ) 
	{
		destroy(_index);
		_index = nullptr;
	}

	if ( _pixels != nullptr )
	{
		zfree(_pixels);
		_pixels = nullptr;
	}
}

int32 CImageAsset::getWidth() const
{
	return _width;
}

int32 CImageAsset::getHeight() const
{
	return _height;
}

int32 CImageAsset::getBytesPerPixel() const
{
	return _bpc;
}

EImagePixelFormat CImageAsset::getPixelFormat() const
{
	return _format;
}

uint8* CImageAsset::getPixels() const
{
	return _pixels;
}

uint32 CImageAsset::getID() const
{
	return _index != nullptr ? _index->get() : 0;
}

void CImageAsset::setImagePixels(EImagePixelFormat format, uint8 bpc, int32 width, int32 height, uint8* pixels)
{
	uint32 size = bpc * width * height;

	_format = format;
	_bpc = bpc;
	_width = width;
	_height = height;

	if ( _pixels != nullptr )
	{
		zfree(_pixels);
	}

	_pixels = (uint8*) zalloc(size);
	memcpy(_pixels, pixels, size);

	invalidate();
}

uint32 CImageAsset::getFlags() const
{
	return _flags;
}

void Arcade::CImageAsset::setFlag(EImageFlags flag, bool enable /*= true*/)
{
	if ( enable )
		_flags |= (uint32)(flag);
	else
		_flags &= !(uint32)(flag);

	invalidate();
}

bool CImageAsset::handleLoadNative()
{
	IRenderer* renderer = gRuntime->getRenderer();
	if ( renderer == nullptr ) return false;

	_nativeData = renderer->getTextureProvider()->loadTexture( renderer, this );

	//log(LOG_MESSAGE, "Load native texture for image: %s", *getName());

	return _nativeData != nullptr;
}

bool CImageAsset::handleUnloadNative()
{
	IRenderer* renderer = gRuntime->getRenderer();
	if ( renderer == nullptr ) return false;

	renderer->getTextureProvider()->freeTexture(renderer, _nativeData);
	_nativeData = nullptr;

	//log(LOG_MESSAGE, "Release native texture for image: %s", *getName());

	return true;
}

IImage::~IImage()
{
}