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
iruntime.h: minimal API-facing runtime object
===============================================================================
*/

#pragma once

namespace Arcade
{

class IRenderer;
class IMetaData;
class IRuntime
{
public:
	virtual bool initialize(class IRuntimeEnvironment* env) = 0;
	virtual class IPackageManager* getPackageManager() = 0;

	virtual void makeCurrent() = 0;
	virtual bool isCurrent() const = 0;

	virtual void tick(float deltatime) = 0;

	virtual IMetaData* createMetaData() = 0;
	virtual void deleteMetaData(IMetaData* data) = 0;

	virtual ISoundMixer* createSoundMixer(CMixerSettings settings) = 0;
	virtual void deleteSoundMixer(ISoundMixer* mixer) = 0;

	virtual class IGameClass* getGameClassForPackage(class IPackage* package) = 0;
	virtual ~IRuntime() {}
};

}