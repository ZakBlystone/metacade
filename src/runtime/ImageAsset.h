/*
* <ImageAsset.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Asset.h"
#include "Runtime.h"

#pragma once

namespace Arcade
{
	//
	//CodeAsset contains a bitmap that can be rendered in OpenGL
	//
	class RUNTIME_API ImageAsset : public GameAsset<ASSET_IMAGE>
	{
	public:
		ImageAsset() : DataBuffer(nullptr) {}

		virtual bool Load(IFileObject *Object);
		virtual bool Save(IFileObject *Object);
		virtual bool Validate();
		virtual void Release();

		void SetPixelsRGBA(void *Data, int ImageWidth, int ImageHeight);	//Set the bitmap
		void *GetPixelsRGBA();												//Get the bitmap

		//Get size of bitmap
		int GetWidth();
		int GetHeight();

	private:

		//Bitmap info
		int Width;
		int Height;

		//Bitmap data
		void *DataBuffer;
		int DataSize;
	};

}