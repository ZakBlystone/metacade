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
metacade_private.h:
===============================================================================
*/

#include "metacade_types.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <sstream>

using std::ostream;
using std::istream;
using std::string;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::list;
using std::make_shared;
using std::make_pair;
using std::enable_shared_from_this;
using std::dynamic_pointer_cast;
using std::sort;
using std::map;
using std::forward;
using std::stringstream;