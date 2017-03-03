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
itexture.h:
===============================================================================
*/

#pragma once

#include "metacade_types.h"

namespace Arcade
{

class ITextureProvider
{
public:
	virtual class ITexture* loadTexture(class IRenderer* renderContext, class IImage* imagesource) = 0;
	virtual void freeTexture(class IRenderer* renderContext, ITexture* texture) = 0;
};

class ITexture
{
public:
	virtual int32 getWidth() const = 0;
	virtual int32 getHeight() const = 0;
	virtual uint16 getID() const = 0;
};

}