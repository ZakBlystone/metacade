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
channel.h:
===============================================================================
*/

#pragma once

#include "engine/private/indexallocator.h"

namespace Arcade
{

class CChannelState
{
	bool _loop;
	float _time;
	float _pitch;
	float _volume;
};

class CSoundChannel : public ISoundChannel
{
public:

	void generatePCM(uint8* buffer, uint32 offset, uint32 size);
	void update();

	uint32 getIndex() const;

private:
	CSoundChannel(shared_ptr<class ISoundSample> sample, shared_ptr<class CSoundMixer> mixer);

	friend class CSoundMixer;

	weak_ptr<class ISoundSample> _sample;
	weak_ptr<class CSoundMixer> _mixer;
	CIndex _index;
};

}