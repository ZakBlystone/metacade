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

Arcade::CSoundMixer::CSoundMixer(CRuntimeObject* outer) 
	: CRuntimeObject(outer)
	, _channelIndices(make_shared<CIndexAllocator>())
	, _maxChannels(32)
{

}

uint32 CSoundMixer::createPersistentChannel()
{
	uint32 newIndex;
	shared_ptr<CSoundChannel> newChannel = addChannel(newIndex, EChannelMode::CHANNELMODE_PERSISTENT);
	return newIndex;
}

void CSoundMixer::freePersistentChannel(uint32 channel)
{
	auto iter = _channels.find(channel);
	if ( iter != _channels.end() )
	{
		_channels.erase( iter );
	}
}

shared_ptr<CSoundChannel> CSoundMixer::createChannelObject(EChannelMode mode /*= CHANNELMODE_DEFAULT*/)
{
	return makeShared<CSoundChannel>( shared_from_this(), lockChannelIndex(), mode );
}

shared_ptr<CSoundChannel> CSoundMixer::addChannel(uint32& index, EChannelMode mode /*= CHANNELMODE_DEFAULT*/)
{
	shared_ptr<CSoundChannel> channelObject = nullptr;
	index = EChannelID::CHANNEL_INVALID;

	if ( _channels.size() >= _maxChannels ) 
	{
		log(LOG_WARN, "Failed to create channel numChannels >= MAX_CHANNELS(%i)", _maxChannels);
		return channelObject;
	}

	channelObject = createChannelObject(mode);
	index = channelObject->getIndex();

	_channels.insert(make_pair(index, channelObject));

	return channelObject;
}

uint32 CSoundMixer::playSound(shared_ptr<ISoundSample> sample, int32 channel /*= EChannelID::CHANNEL_ANY*/)
{
	shared_ptr<CSoundChannel> channelObject = nullptr;
	uint32 newIndex = EChannelID::CHANNEL_INVALID;

	if ( channel == -1 )
	{
		channelObject = addChannel(newIndex);
	}
	else
	{
		auto iter = _channels.find(channel);
		if ( iter == _channels.end() )
		{
			return EChannelID::CHANNEL_INVALID;
		}

		channelObject = (*iter).second;
		newIndex = channel;
	}

	if ( channelObject == nullptr )
	{
		return EChannelID::CHANNEL_INVALID;
	}

	channelObject->play(sample);

	return newIndex;
}

CIndex CSoundMixer::lockChannelIndex()
{
	return _channelIndices->getIndex();
}
