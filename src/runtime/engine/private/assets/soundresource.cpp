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

Arcade::CSoundAsset::CSoundAsset() 
	: _waveData(nullptr)
	, _waveSize(0)
{

}

Arcade::CSoundAsset::~CSoundAsset()
{
	release();
}

bool Arcade::CSoundAsset::load(IFileObject* file)
{
	if ( !file->read(&_sampleInfo, sizeof(CSampleInfo)) ) return false;
	if ( !file->read(&_waveSize, sizeof(uint32)) ) return false;
	
	if ( _waveData != nullptr )
		release();

	_waveData = (uint8*) zalloc(_waveSize);

	if ( !file->read(_waveData, _waveSize) ) return false;

	return true;
}

bool Arcade::CSoundAsset::save(IFileObject* file)
{	
	if ( !file->write(&_sampleInfo, sizeof(CSampleInfo)) ) return false;
	if ( !file->write(&_waveSize, sizeof(uint32)) ) return false;
	
	if ( _waveData == nullptr ) return false;

	if ( !file->write(_waveData, _waveSize) ) return false;

	return true;
}

bool Arcade::CSoundAsset::validate() const
{
	return true;
}

void Arcade::CSoundAsset::release()
{
	if ( _waveData != nullptr )
	{
		zfree(_waveData);
		_waveData = nullptr;
	}
}

void Arcade::CSoundAsset::setWaveData(uint8* waveData, uint32 waveSize)
{
	if ( waveSize == 0 ) return;

	if ( _waveData != nullptr )
	{
		release();
	}

	_waveSize = waveSize;
	_waveData = (uint8*) zalloc(_waveSize);

	memcpy(_waveData, waveData, waveSize * sizeof(uint8));
}

void Arcade::CSoundAsset::setSampleInfo(const CSampleInfo& info)
{
	_sampleInfo = info;
}

void Arcade::CSoundAsset::getSampleInfo(CSampleInfo& info)
{
	info = _sampleInfo;
}

uint8* Arcade::CSoundAsset::getPCMSamples()
{
	return _waveData;
}
