/*
* <SoundAsset.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Asset.h"
#include "Runtime.h"

#pragma once

namespace Arcade
{

	class RUNTIME_API SoundAsset : public GameAsset<ASSET_SOUND>
	{
	public:
		SoundAsset() : DataBuffer(nullptr) {}

		virtual bool Load(IFileObject *Object);
		virtual bool Save(IFileObject *Object);
		virtual bool Validate();
		virtual void Release();

		void SetWaveSamples(char *WaveBuffer, int SampleRate, int Samples, bool Signed);

	private:

		void *DataBuffer;
		int SampleRate;
		int Samples;
		bool Signed;
	};

}