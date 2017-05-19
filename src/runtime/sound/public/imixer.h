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
imixer.h:
===============================================================================
*/

#pragma once

#define MIXF_STEREO 1 << 0
#define MIXF_ALIASED_INTERPOLATION 1 << 1
#define MIXF_8BIT 1 << 2
#define MIXF_DEFAULT MIXF_STEREO

namespace Arcade
{

struct CMixerSettings
{
	CMixerSettings()
		: flags(MIXF_DEFAULT)
		, sampleRate(44100)
		, bufferSize(1024)
		, maxChannels(32)
	{}

	int32 flags;
	int32 sampleRate;
	int32 bufferSize;
	int32 maxChannels;

	inline int32 getBytesPerFrame() const
	{
		return ((flags & MIXF_STEREO) ? 2 : 1) * ((flags & MIXF_8BIT) ? 1 : 2);
	}

	inline int32 getChannelCount() const
	{
		return ((flags & MIXF_STEREO) ? 2 : 1);
	}
};

class ISoundMixer
{
public:
	virtual uint32 playSound(const class CAssetRef &sound, int32 channel = EChannelID::CHANNEL_ANY) = 0;
	virtual void stopSound(int32 channel) = 0;

	virtual uint32 createPersistentChannel() = 0;
	virtual void destroyPersistentChannel(uint32 channel) = 0;

	virtual void update() = 0;

	virtual uint8* getPCMSamples() = 0;
	virtual uint32 availablePCMData() = 0;

	virtual void setChannelPitch(int32 channel, float pitch) = 0;
	virtual void setChannelLooping(int32 channel, bool loop) = 0;
	virtual void setChannelVolume(int32 channel, float volume) = 0;
};

}