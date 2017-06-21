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
random.cpp: Pseudo-random number generator
===============================================================================
*/

#include "core_private.h"

CRandom::CRandom()
	: _seedX(1503829603)
	, _seedY(4028562812)
{

}

uint32 CRandom::random()
{
	static uint32 a = 30963;
	static uint32 b = 21723;

	_seedX = a * (_seedX & 0xFFFF) + (_seedX >> 16);
	_seedY = b * (_seedY & 0xFFFF) + (_seedY >> 16);

	return (_seedX << 16) + (_seedY & 0xFFFF);
}

uint32 CRandom::random(uint32 maximum)
{
	return random() % maximum;
}

int32 CRandom::randomInt(int32 minimum, int32 maximum)
{
	return minimum + random(maximum - minimum + 1);
}

float CRandom::randomFloat()
{
	uint32 uint = random();
	return (float) (uint) / 0xFFFFFFFF;
}

void CRandom::randomSeed(int32 newSeed)
{
	_seedY = newSeed;
}
