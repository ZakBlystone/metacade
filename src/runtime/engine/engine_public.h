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
engine_public.h:
===============================================================================
*/

#include "core/core_public.h"
#include "sound/sound_public.h"

#include "public/inputevent.h"
#include "public/inputstate.h"
#include "public/functioncall.h"
#include "public/igame.h"
#include "public/iruntime.h"
#include "public/ilogger.h"
#include "public/iallocator.h"
#include "public/ifilesystem.h"
#include "public/imetadata.h"
#include "public/imachineenvironment.h"
#include "public/iruntimeenvironment.h"
#include "public/ipackagemanager.h"
#include "public/runtimeobject.h"
#include "public/asset.h"
#include "public/packagebuilder.h"
#include "public/api.h"

#include "public/assets/scriptresource.h"
#include "public/assets/imageresource.h"
#include "public/assets/soundresource.h"
