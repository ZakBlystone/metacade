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

CSoundChannel::CSoundChannel(CSoundMixer *mixer, CIndex index, EChannelMode mode /*= CHANNELMODE_DEFAULT*/)
	: _mixer(mixer)
	, _index(index)
	, _mode(mode)
{

}

void Arcade::CSoundChannel::play(shared_ptr<class ISoundSample> sample)
{
	if ( sample == nullptr ) return;

	sample->getSampleInfo(_sampleInfo);

	_rate = (float) _sampleInfo.sampleRate;
	if ( _rate <= 0.f ) return;

	_sample = sample;
	_invRate = 1.f / _rate;
	_stride = _sampleInfo.numChannels * _sampleInfo.width;

	_state._time = 0.f;
	_state._duration = (float)(_sampleInfo.numFrames) * _invRate;
}

EChannelMode CSoundChannel::getMode() const
{
	return _mode;
}

Arcade::CChannelState& Arcade::CSoundChannel::getState()
{
	return _state;
}

void CSoundChannel::update()
{

}

uint32 CSoundChannel::getIndex() const
{
	return _index.get();
}

bool CSoundChannel::generateSinglePCMSample(float* buffer, uint32 offset, ISoundSample* sample, const CMixerSettings& mixerSettings)
{
	//playhead
	float time = _rate * _state._time;
	uint32 index = (uint32) floorf(time);
	float frac = time - (float)(index);

	//out of bounds
	if ( index + 1 > _sampleInfo.numFrames && !_state._loop )
		return false;

	//interpolation points
	uint32 frame = index;
	uint32 nextFrame = index + 1;

	if ( frame > _sampleInfo.numFrames )
		return false;

	//interpolate back to first sample if looping
	if ( nextFrame >= _sampleInfo.numFrames )
		nextFrame = 0;
	
	//sample locations
	int32 sampleIndex = frame * _sampleInfo.numChannels;
	int32 nextSampleIndex = frame * _sampleInfo.numChannels;

	float vol = _state._volume;

	//interpolation
	if ( _sampleInfo.width == 16 )
	{
		int16* sampleData = reinterpret_cast<int16*>(sample->getPCMSamples());
		if ( _sampleInfo.numChannels == 1 )
		{
			float A = (float)(sampleData[sampleIndex]);
			float B = (float)(sampleData[nextSampleIndex]);
			float lerped = A + (B - A) * frac;
			if ( mixerSettings.flags & MIXF_STEREO )
			{
				buffer[offset] += lerped * vol;
				buffer[offset+1] += lerped * vol;
			}
			else
			{
				buffer[offset] += lerped * vol;	
			}
		}
		else if ( _sampleInfo.numChannels = 2 )
		{
			float AL = (float)(sampleData[sampleIndex]);
			float AR = (float)(sampleData[sampleIndex+1]);
			float BL = (float)(sampleData[nextSampleIndex]);
			float BR = (float)(sampleData[nextSampleIndex+1]);
			float lerpedL = AL + (BL - AL) * frac;
			float lerpedR = AR + (BR - AR) * frac;
			if ( mixerSettings.flags & MIXF_STEREO )
			{
				buffer[offset] += lerpedL * vol;
				buffer[offset+1] += lerpedR * vol;
			}
			else
			{
				buffer[offset] += (lerpedR + lerpedL) * .5f * vol;
			}
		}
	}

	return true;
}

bool CSoundChannel::generatePCM(float* buffer, uint32 offset, uint32 size)
{
	shared_ptr<ISoundSample> sample = _sample.lock();
	if ( _mixer == nullptr || sample == nullptr ) return false;

	const CMixerSettings& settings = _mixer->getSettings();
	ISoundSample* sampleptr = sample.get();

	int32 channelCount = settings.getChannelCount();

	float rateFactor = _invRate * _state._pitch * (_rate / settings.sampleRate);
	bool generatedSample = false;

	for ( uint32 i=offset; i<offset + size; i+=channelCount )
	{
		generatedSample |= generateSinglePCMSample(buffer, i, sampleptr, settings);
		if ( !generatedSample ) return false;

		_state._time += rateFactor;

		if ( _state._loop && _state._time >= _state._duration )
		{
			_state._time = 0.f;
		}
	}

	return generatedSample;
}