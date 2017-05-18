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
ivmclass.h: Game archetype
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

class IVMClass
{
public:
	virtual bool reload() = 0;

	virtual class IVMHost* getHost() = 0;
	virtual shared_ptr<class CMetaData> getMetaData() = 0;
	virtual shared_ptr<class IVMInstance> createVMInstance() = 0;
};

}