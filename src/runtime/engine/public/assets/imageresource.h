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
imageresource.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CImageAsset : public CNativeLoadableAsset<ASSET_TEXTURE, ITexture>, public IImage
{
public:
	CImageAsset();
	virtual ~CImageAsset();

	virtual bool load(IFileObject* file) override;
	virtual bool save(IFileObject* file) override;
	virtual bool validate() const override;
	virtual void release() override;

	virtual int32 getWidth() const override;
	virtual int32 getHeight() const override;
	virtual int32 getBytesPerPixel() const override;

	virtual EImagePixelFormat getPixelFormat() const override;

	virtual uint8* getPixels() const override;
	virtual uint32 getID() const override;
	virtual uint32 getFlags() const override;

	void setImagePixels(EImagePixelFormat format, uint8 bpc, int32 width, int32 height, uint8* pixels);
	void setFlag(EImageFlags flag, bool enable = true);

private:
	class CIndex *_index;

	EImagePixelFormat _format;
	uint8 _bpc;
	int32 _width;
	int32 _height;
	uint32 _flags;
	uint8* _pixels;
protected:

	virtual bool handleLoadNative() override;
	virtual bool handleUnloadNative() override;

};

}