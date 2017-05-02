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
channel.cpp:
===============================================================================
*/

#include "sound_private.h"

CSoundChannel::CSoundChannel(shared_ptr<CSoundMixer> mixer, CIndex index, EChannelMode mode /*= CHANNELMODE_DEFAULT*/)
	: _mixer(mixer)
	, _index(index)
	, _mode(mode)
{

}

void Arcade::CSoundChannel::play(shared_ptr<class ISoundSample> sample)
{

}

EChannelMode CSoundChannel::getMode() const
{
	return _mode;
}

void CSoundChannel::generatePCM(uint8* buffer, uint32 offset, uint32 size)
{

}

void CSoundChannel::update()
{

}

uint32 CSoundChannel::getIndex() const
{
	return _index.get();
}
