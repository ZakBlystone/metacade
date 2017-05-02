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
soundresource.cpp:
===============================================================================
*/

#include "engine/engine_private.h"

Arcade::CSoundAsset::CSoundAsset(CRuntimeObject* outer) 
	: CAsset(outer)
{

}

bool Arcade::CSoundAsset::load(IFileObject* file)
{
	return false;
}

bool Arcade::CSoundAsset::save(IFileObject* file)
{
	return false;
}

bool Arcade::CSoundAsset::validate() const
{
	return false;
}

void Arcade::CSoundAsset::release()
{

}

void Arcade::CSoundAsset::getSampleInfo(CSampleInfo& info)
{

}

uint8* Arcade::CSoundAsset::getPCMSamples()
{
	return nullptr;
}
