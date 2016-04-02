/*
* <CodeAsset.h>
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
	//CodeAsset contains Lua code to be executed at runtime.
	//
	class RUNTIME_API CodeAsset : public GameAsset<ASSET_CODE>
	{
	public:
		CodeAsset() : CodeBuffer(nullptr) {}

		virtual bool Load(IFileObject *Object);
		virtual bool Save(IFileObject *Object);
		virtual bool Validate();
		virtual void Release();

		const char *GetCodeBuffer();					//Returns a pointer to the code section of this asset
		void SetCodeBuffer(char *Buffer, int Size);		//Puts code into the code section of this asset

	private:

		char *CodeBuffer;		//Pointer to code string
		int CodeBufferSize;		//Size of code string
	};

}