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
sha.h: 
===============================================================================
*/

/*
  100% free public domain implementation of the SHA-1 algorithm
  by Dominik Reichl <dominik.reichl@t-online.de>
  Web: http://www.dominik-reichl.de/
*/

#pragma once

namespace Arcade
{

class CSHA1
{
public:
	CSHA1();

	typedef union
	{
		uint8 c[64];
		uint32 l[16];
	} WorkspaceBlock;

	class Generator
	{
	public:
		Generator()
			: _block ((WorkspaceBlock* ) _workspace)
		{}

		void init();
		void update(const void* data, uint32 length);
		void done();
		CSHA1 get() const;

	private:
		void transform(uint32* state, const uint8* buffer);
		uint32 _state[5];
		uint32 _count[2];
		uint32 reserved0;
		uint8 _buffer[64];
		uint8 _digest[20];
		uint32 _reserved1[3];
		uint8 _workspace[64];
		WorkspaceBlock* _block;
	};

	void getDigest(uint8 digest[20]) const;
	CString tostring() const;

private:
	uint8 _digest[20];
};

}