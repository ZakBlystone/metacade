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
ivminstance.h: Runnable game code instance
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

class IVMInstance
{
public:
	virtual class IVMClass* getClass() = 0;

	virtual void setMachineEnvironment(IMachineEnvironment *env) = 0;

	virtual bool postCommand(const char** commandBuffer) = 0;
	virtual void postInputEvent(const class CInputEvent& input) = 0;

	virtual void precacheAssets() = 0;
	virtual void think(float seconds, float deltaSeconds) = 0;
	virtual void render(class CElementRenderer* renderer) = 0;

	virtual void reset() = 0;
};

}