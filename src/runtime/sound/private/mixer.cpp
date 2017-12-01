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
mixer.cpp:
===============================================================================
*/

#include "sound_private.h"

Arcade::CSoundMixer::CSoundMixer(CRuntimeObject* outer, CMixerSettings settings /*= CMixerInitSettings()*/) 
	: CRuntimeObject(outer)
	, _channelIndices(make_shared<CIndexAllocator>())
	, _settings(settings)
	, _available(0)
	, _pitch(1.f)
	, _volume(1.f)
{
	_mixBuffer = shared_ptr<float>( 
		(float*) zalloc( _settings.bufferSize * sizeof(float) * _settings.getChannelCount() ),
		[this](float* del) { this->zfree(del); }
	);

	_outBuffer = shared_ptr<uint8>( 
		(uint8*) zalloc( _settings.bufferSize * sizeof(uint8) * _settings.getBytesPerFrame() ),
		[this](uint8* del) { this->zfree(del); }
	);
}

uint32 Arcade::CSoundMixer::playSound(const CAssetRef& sound, int32 channel /*= EChannelID::CHANNEL_ANY*/)
{
	CPackage* pkg = (CPackage*) (sound.getPackage(getRuntime()));
	if ( pkg != nullptr )
	{
		shared_ptr<IAsset> asset = pkg->getAssetMap()->findAssetByID(sound.getAssetID());
		if ( asset == nullptr )
		{
			log(LOG_ERROR, "Sound asset is null");
			return (uint32) EChannelID::CHANNEL_INVALID;	
		}
		if ( asset->getType() != ASSET_SOUND )
		{
			log(LOG_ERROR, "Asset is not a sound");
			return (uint32) EChannelID::CHANNEL_INVALID;
		}

		return playSoundSample(dynamic_pointer_cast<ISoundSample>( asset ), channel);
	}

	return (uint32) EChannelID::CHANNEL_INVALID;
}

void CSoundMixer::stopSound(int32 channel)
{
	auto iter = _channels.find(channel);
	if ( iter != _channels.end() )
	{
		_channels.erase( iter );
	}
}

void CSoundMixer::update()
{
	float* buffer = _mixBuffer.get();
	int16* outbuf = (int16*) _outBuffer.get();

	uint32 realSize = _settings.bufferSize * _settings.getChannelCount();

	for ( uint32 i=0; i<realSize; ++i )
	{
		buffer[i] = 0.f;
	}

	for ( auto it = _channels.cbegin(); it != _channels.cend(); )
	{
		if ( !(*it).second->generatePCM( buffer, 0, realSize ) )
		{
			it = _channels.erase(it);
		}
		else
		{
			++it;
		}
	}

	for ( uint32 i=0; i<realSize; ++i )
	{
		outbuf[i] = (int16)( min(max(buffer[i], -32678.f),32767.f) );
	}
}

uint8* CSoundMixer::getPCMSamples()
{
	return _outBuffer.get();
}

uint32 CSoundMixer::availablePCMData()
{
	return _available;
}

void CSoundMixer::setChannelPitch(int32 channel, float pitch)
{
	auto iter = _channels.find(channel);
	if ( iter != _channels.end() )
	{
		(*iter).second->getState()._pitch = pitch;
	}
}

void CSoundMixer::setChannelLooping(int32 channel, bool loop)
{
	auto iter = _channels.find(channel);
	if ( iter != _channels.end() )
	{
		(*iter).second->getState()._loop = loop;
	}
}

void CSoundMixer::setChannelVolume(int32 channel, float volume)
{
	auto iter = _channels.find(channel);
	if ( iter != _channels.end() )
	{
		(*iter).second->getState()._volume = volume;
	}
}

void CSoundMixer::setMasterPitch(float pitch)
{
	_pitch = pitch;
}

void CSoundMixer::setMasterVolume(float volume)
{
	_volume = volume;
}

const CMixerSettings& CSoundMixer::getSettings() const
{
	return _settings;
}

uint32 CSoundMixer::createPersistentChannel()
{
	uint32 newIndex;
	shared_ptr<CSoundChannel> newChannel = addChannel(newIndex, EChannelMode::CHANNELMODE_PERSISTENT);
	return newIndex;
}

void CSoundMixer::destroyPersistentChannel(uint32 channel)
{
	auto iter = _channels.find(channel);
	if ( iter != _channels.end() )
	{
		_channels.erase( iter );
	}
}

shared_ptr<CSoundChannel> CSoundMixer::createChannelObject(EChannelMode mode /*= CHANNELMODE_DEFAULT*/)
{
	return makeShared<CSoundChannel>( this, lockChannelIndex(), mode );
}

shared_ptr<CSoundChannel> CSoundMixer::addChannel(uint32& index, EChannelMode mode /*= CHANNELMODE_DEFAULT*/)
{
	shared_ptr<CSoundChannel> channelObject = nullptr;
	index = (uint32) EChannelID::CHANNEL_INVALID;

	if ( _channels.size() >= _settings.maxChannels ) 
	{
		log(LOG_WARN, "Failed to create channel numChannels >= %i", _settings.maxChannels);
		return channelObject;
	}

	channelObject = createChannelObject(mode);
	index = channelObject->getIndex();

	_channels.insert(make_pair(index, channelObject));

	return channelObject;
}

uint32 CSoundMixer::playSoundSample(shared_ptr<ISoundSample> sample, int32 channel /*= EChannelID::CHANNEL_ANY*/)
{
	shared_ptr<CSoundChannel> channelObject = nullptr;
	uint32 newIndex = (uint32) EChannelID::CHANNEL_INVALID;

	if ( channel == -1 )
	{
		channelObject = addChannel(newIndex);
	}
	else
	{
		auto iter = _channels.find(channel);
		if ( iter == _channels.end() )
		{
			return (uint32) EChannelID::CHANNEL_INVALID;
		}

		channelObject = (*iter).second;
		newIndex = channel;
	}

	if ( channelObject == nullptr )
	{
		return (uint32) EChannelID::CHANNEL_INVALID;
	}

	channelObject->play(sample);

	return newIndex;
}

CIndex CSoundMixer::lockChannelIndex()
{
	return _channelIndices->getIndex();
}
