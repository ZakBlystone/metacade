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

struct CInterpolatorData
{
	int32 channels;
	int32 current;
	int32 next;
	float frac;
	float vol;
	float* outbuffer;
	bool stereo;
	bool aliasing;
};

template<typename SampleType>
static inline float lerpSamples(const SampleType* samples, int32 current, int32 next, float frac, bool alias)
{
	float A = (float)(samples[current]); if ( alias ) return A;
	float B = (float)(samples[next]);
	return A + (B - A) * frac;
}

template<>
inline float lerpSamples<int8>(const int8* samples, int32 current, int32 next, float frac, bool alias)
{
	float A = (float)(samples[current] << 8); if ( alias ) return A;
	float B = (float)(samples[next] << 8);
	return A + (B - A) * frac;
}

template<typename SampleType, int32 ChannelCount>
struct CSampleInterpolator
{;
	inline void stereoInterpolate(	
		const SampleType* samples, 
		const CInterpolatorData& data)
	{}
};

template<typename SampleType>
struct CSampleInterpolator<SampleType, 2>
{
	static inline void stereoInterpolate(
		const SampleType* samples, 
		const CInterpolatorData& data)
	{
		float lerpedL = lerpSamples<SampleType>(samples, data.current, data.next, data.frac, data.aliasing);
		float lerpedR = lerpSamples<SampleType>(samples, data.current+1, data.next+1, data.frac, data.aliasing);
		if ( data.stereo )
		{
			data.outbuffer[0] += lerpedL * data.vol;
			data.outbuffer[1] += lerpedR * data.vol;
		}
		else
		{
			data.outbuffer[0] += (lerpedR + lerpedL) * .5f * data.vol;
		}
	}
};

template<typename SampleType>
struct CSampleInterpolator<SampleType, 1>
{
		static inline void stereoInterpolate(
		const SampleType* samples, 
		const CInterpolatorData& data)
	{
		float lerped = lerpSamples<SampleType>(samples, data.current, data.next, data.frac, data.aliasing);
		if ( data.stereo )
		{
			data.outbuffer[0] += lerped * data.vol;
			data.outbuffer[1] += lerped * data.vol;
		}
		else
		{
			data.outbuffer[0] += lerped * data.vol;
		}
	}
};

template<typename SampleType>
static inline bool interpolateSamples(const uint8* buffer, const CInterpolatorData& data)
{
	const SampleType* sampleData = reinterpret_cast<const SampleType*>(buffer);
	if ( sampleData == nullptr ) return false;

	if ( data.channels == 1 )
	{
		CSampleInterpolator<SampleType,1>::stereoInterpolate(sampleData, data);
	}
	else if ( data.channels == 2 )
	{
		CSampleInterpolator<SampleType,2>::stereoInterpolate(sampleData, data);
	}

	return true;
}

bool CSoundChannel::generateSinglePCMSample(float* buffer, uint32 offset, ISoundSample* sample, const CMixerSettings& mixerSettings)
{
	//data for interpolator
	static CInterpolatorData interpolate;
	const CMixerSettings& settings = _mixer->getSettings();

	//playhead
	double time = _rate * _state._time;
	uint32 index = (uint32) floor(time);
	float frac = (float)time - (float)(index);

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
	
	//setup interpolator data
	interpolate.current = frame * _sampleInfo.numChannels;
	interpolate.next = nextFrame * _sampleInfo.numChannels;
	interpolate.frac = frac;
	interpolate.outbuffer = buffer + offset;
	interpolate.stereo = settings.flags & MIXF_STEREO;
	interpolate.vol = _state._volume * _mixer->getMasterVolume();
	interpolate.channels = _sampleInfo.numChannels;
	interpolate.aliasing = settings.flags & MIXF_ALIASED_INTERPOLATION;

	//interpolation
	if ( _sampleInfo.width == 16 )
	{
		interpolateSamples<int16>(sample->getPCMSamples(), interpolate );
	}
	else if ( _sampleInfo.width == 8 )
	{
		interpolateSamples<int8>(sample->getPCMSamples(), interpolate );
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

	double rateFactor = _invRate * _state._pitch * (_rate / settings.sampleRate) * _mixer->getMasterPitch();
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