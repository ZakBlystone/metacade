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
mixer.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CSoundMixer : public ISoundMixer
{
public:
	CSoundMixer(CMixerSettings settings = CMixerSettings());

	//returns channel index
	uint32 playSoundSample(shared_ptr<ISoundSample> sample, int32 channel = EChannelID::CHANNEL_ANY);

	virtual uint32 createPersistentChannel();
	virtual void destroyPersistentChannel(uint32 channel);

	virtual uint32 playSound(const CAssetRef& sound, int32 channel = EChannelID::CHANNEL_ANY) override;
	virtual void stopSound(int32 channel) override;

	virtual void update();

	virtual uint8* getPCMSamples();
	virtual uint32 availablePCMData();

	virtual void setChannelPitch(int32 channel, float pitch);
	virtual void setChannelLooping(int32 channel, bool loop);
	virtual void setChannelVolume(int32 channel, float volume);

	virtual void setMasterPitch(float pitch);
	virtual void setMasterVolume(float volume);

	float getMasterPitch() const { return _pitch; }
	float getMasterVolume() const { return _volume; }

	const CMixerSettings& getSettings() const;

private:
	CIndex lockChannelIndex();

	shared_ptr<CSoundChannel> createChannelObject(EChannelMode mode = CHANNELMODE_DEFAULT);
	shared_ptr<CSoundChannel> addChannel(uint32& index, EChannelMode mode = CHANNELMODE_DEFAULT);

	map<uint32, shared_ptr<CSoundChannel>> _channels;
	shared_ptr<CIndexAllocator> _channelIndices;

	CMixerSettings _settings;
	uint32 _available;

	shared_ptr<float> _mixBuffer;
	shared_ptr<uint8> _outBuffer;

	float _pitch;
	float _volume;
};

}