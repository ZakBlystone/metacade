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
compiler.cpp: Compiles IAssets
===============================================================================
*/

#include "metacade_amal.h"

using namespace Arcade;

#include "IL/il.h"
#include "compiler.h"

static bool buildImage(CImageAsset* asset, IMetaData* params)
{
	ILuint test = ilGenImage();
	ilBindImage(test);
	if ( !ilLoadImage(*params->getValue("file")) ) return false;

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int width = (int) ilGetInteger(IL_IMAGE_WIDTH);
	int height = (int) ilGetInteger(IL_IMAGE_HEIGHT);
	
	uint8* data = (uint8*) ilGetData();
	asset->setImagePixels(PFM_RGBA8, 4, width, height, data);
	return true;
}

CCompiler::CCompiler()
{
	ilInit();
}

CCompiler::~CCompiler()
{
	ilShutDown();
}

bool CCompiler::compile(IAsset* asset, IMetaData* buildParameters)
{
	switch (asset->getType())
	{
	case ASSET_NONE:
	break;
	case ASSET_CODE:
	break;
	case ASSET_TEXTURE:
	return buildImage((CImageAsset*)asset, buildParameters);
	break;
	case ASSET_SOUND:
	break;
	default:
	break;
	}

	return false;
}
