/*
* <SoundAsset.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "SoundAsset.h"
#include "Runtime.h"
#include "FileInterface.h"
#include <string.h>

using namespace Arcade;

bool SoundAsset::Load(IFileObject *Object)
{
	if (!Object->Read(&SampleRate, sizeof(int))) return false;
	if (!Object->Read(&Samples, sizeof(int))) return false;
	if (!Object->Read(&Signed, sizeof(bool))) return false;

	DataBuffer = Runtime::Realloc(DataBuffer, Samples * sizeof(unsigned short));

	if (!DataBuffer) return false;
	if (!Object->Read(DataBuffer, Samples * sizeof(unsigned short))) return false;

	return true;
}

bool SoundAsset::Save(IFileObject *Object)
{
	if (!Object->Write(&SampleRate, sizeof(int))) return false;
	if (!Object->Write(&Samples, sizeof(int))) return false;
	if (!Object->Write(&Signed, sizeof(bool))) return false;

	if (!DataBuffer) return false;
	if (!Object->Write(DataBuffer, Samples * sizeof(unsigned short))) return false;

	return true;
}

bool SoundAsset::Validate()
{
	return true;
}

void SoundAsset::Release()
{
	Runtime::Free(DataBuffer);
	DataBuffer = nullptr;
}

void SoundAsset::SetWaveSamples(char *WaveBuffer, int SampleRate, int Samples, bool Signed)
{
	DataBuffer = Runtime::Realloc(DataBuffer, Samples * sizeof(unsigned short));

	memcpy(DataBuffer, WaveBuffer, Samples * sizeof(unsigned short));

	this->SampleRate = SampleRate;
	this->Samples = Samples;
	this->Signed = Signed;
}
