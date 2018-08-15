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
igame.h: interfaces for internal game types
===============================================================================
*/

#pragma once

namespace Arcade
{

class IGameClass
{
public:
	virtual bool createInstance(class IGameInstance** instance, void* userdata = nullptr) = 0;
	virtual void deleteInstance(class IGameInstance* instance) = 0;
	virtual ~IGameClass() {};
};

class IHostCallbacks
{
public:
	virtual bool handleHostFunctionCall(const CFunctionCall& call, CVariant& returnValue) = 0;
	virtual ~IHostCallbacks() {};
};

class IGameInstance
{
public:
	virtual class IGameClass* getClass() = 0;

	virtual void* getUserData() = 0;

	virtual void postInputEvent(const class CInputEvent& input) = 0;
	virtual void postInputState(const class CInputState& input) = 0;
	virtual void init() = 0;
	virtual void render(CVec2 viewportSize) = 0;
	virtual bool callFunction(const CFunctionCall& call) = 0;

	virtual void initSoundMixer(const CMixerSettings& settings) = 0;
	virtual ISoundMixer* getSoundMixer() = 0;

	virtual void setHostCallbacks(IHostCallbacks* callbacks) = 0;
	virtual ~IGameInstance() {};
};

}