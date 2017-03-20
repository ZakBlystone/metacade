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
ivmhost.h: Creates VMs
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

class IVMHost
{
public:
	virtual ELanguage getLanguage() = 0;

	virtual bool init() = 0;
	virtual void shutdown() = 0;
	virtual bool isRunning() = 0;

	virtual weak_ptr<IVMClass> loadGameVMClass() = 0;
	virtual weak_ptr<IVMClass> loadGameVMClass(const class CCodeAsset* codeAsset) = 0;

	virtual bool includeGameScript() = 0;
	virtual bool validateGameScript() = 0;
};

}