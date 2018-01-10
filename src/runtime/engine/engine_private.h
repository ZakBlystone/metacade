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
engine_private.h:
===============================================================================
*/

#define ENGINE_PRIVATE 1

#include "core_private.h"
#include "engine_public.h"

#include "public/asset.h"

#include "private/ivminstance.h"
#include "private/ivmclass.h"
#include "private/ivmhost.h"

#include "private/indexallocator.h"
#include "private/fileutil.h"
#include "private/metadata.h"
#include "private/filecollection.h"
#include "private/assetmap.h"
#include "private/package.h"
#include "private/packagemanager.h"
#include "private/runtime.h"
#include "private/gameclass.h"
#include "private/gameinstance.h"

using namespace Arcade;