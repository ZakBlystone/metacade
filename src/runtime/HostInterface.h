/*
* <HostInterface.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "FileInterface.h"

#pragma once

namespace Arcade
{
	//
	//Interface with the host application for building assets from files
	//
	class IHostBuildInterface
	{
	public:
		virtual bool CompileImageAsset(const char *Filename, class ImageAsset *Image) = 0;
		virtual bool CompileSoundAsset(const char *Filename, class SoundAsset *Sound) = 0;
	};

	//
	//Main Interface with the host application
	//
	class IHostInterface
	{
	public:
		virtual IHostFileInterface *GetFileInterface() = 0;
		virtual IHostBuildInterface *GetBuildInterface() { return nullptr; } //not required
	};

}