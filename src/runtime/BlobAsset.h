/*
* <BlobAsset.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Asset.h"
#include "Runtime.h"

#pragma once

namespace Arcade
{
	/*
	------------------------
	This class is not implemented yet, it is meant to be an asset for storing arbitrary binary data.
	------------------------
	*/

	class RUNTIME_API BlobAsset : public GameAsset<ASSET_BLOB>
	{
	public:
		virtual bool Load(IFileObject *Object);
		virtual bool Save(IFileObject *Object);
		virtual bool Validate();
		virtual void Release();

	private:

		char *DataBuffer;
		int DataSize;
	};

}