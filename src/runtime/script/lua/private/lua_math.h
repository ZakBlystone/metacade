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
lua_math.h:
===============================================================================
*/

#pragma once

#include "core/core_public.h"

namespace Arcade
{

extern bool lua_toVec2(lua_State *L, int idx, CVec2& vec);
extern void lua_pushVec2(lua_State *L, const CVec2& vec);
extern const CMatrix3& lua_toMatrix3x3(lua_State *L, int idx);
extern void lua_pushMatrix3x3(lua_State *L, const CMatrix3& matrix);

extern void OpenLuaMathModule(lua_State *L);

}