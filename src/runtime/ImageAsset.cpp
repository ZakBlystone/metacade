/*
* <ImageAsset.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "ImageAsset.h"
#include "Runtime.h"
#include <string.h>

using namespace Arcade;

bool ImageAsset::Load(IFileObject *Object)
{
	//Read width and height from file
	if (!Object->Read(&Width, sizeof(int))) return false;
	if (!Object->Read(&Height, sizeof(int))) return false;

	//Allocate the bitmap
	DataBuffer = Runtime::Realloc(DataBuffer, Width * Height * 4);

	//Fail if bitmap didn't allocate
	if (!DataBuffer) return false;

	//Read bitmap from file
	if (!Object->Read(DataBuffer, Width * Height * 4)) return false;

	return true;
}

bool ImageAsset::Save(IFileObject *Object)
{
	//Write width and height from file
	if (!Object->Write(&Width, sizeof(int))) return false;
	if (!Object->Write(&Height, sizeof(int))) return false;

	//Fail if bitmap doesn't exist
	if (!DataBuffer) return false;

	Runtime::LogPrint(LOG_MESSAGE, "Write Image Bytes: %i", Width * Height * 4);

	if ( Width > 4096 || Height > 4096 ) 
	{
		Runtime::LogPrint(LOG_ERROR, "Image Is Too Big: %ix%i, max is 4096x4096", Width, Height);
		return false;
	}

	unsigned int Size = Width * Height * 4 * sizeof(char);

	//Write bitmap to file
	if (!Object->Write(DataBuffer, Size)) return false;

	return true;
}

bool ImageAsset::Validate()
{
	return true;
}

void ImageAsset::Release()
{
	//Free the bitmap from memory
	Runtime::Free(DataBuffer);
	DataBuffer = nullptr;
}

void ImageAsset::SetPixelsRGBA(void *Data, int ImageWidth, int ImageHeight)
{
	//Set width and height
	Width = ImageWidth;
	Height = ImageHeight;

	unsigned int Size = Width * Height * 4 * sizeof(char);

	Runtime::LogPrint(LOG_MESSAGE, "Image Bytes: %i", Size);

	//Allocate a buffer and copy the bitmap into this Asset
	DataBuffer = Runtime::Realloc(DataBuffer, Size);
	memcpy(DataBuffer, Data, Size);
}

//Get the bitmap
void * ImageAsset::GetPixelsRGBA()
{
	return DataBuffer;
}

//Get size of bitmap
int ImageAsset::GetWidth()
{
	return this->Width;
}

//Get size of bitmap
int ImageAsset::GetHeight()
{
	return this->Height;
}
