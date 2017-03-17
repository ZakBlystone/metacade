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
sha.cpp: 
===============================================================================
*/

/*
  100% free public domain implementation of the SHA-1 algorithm
  by Dominik Reichl <dominik.reichl@t-online.de>
  Web: http://www.dominik-reichl.de/
*/

#include "core_private.h"

#define ROL32(p_val32,p_nBits) (((p_val32)<<(p_nBits))|((p_val32)>>(32-(p_nBits))))
#define SHABLK0(i) (_block->l[i] = \
	(ROL32(_block->l[i],24) & 0xFF00FF00) | (ROL32(_block->l[i],8) & 0x00FF00FF))

#define SHABLK(i) (_block->l[i&15] = ROL32(_block->l[(i+13)&15] ^ \
	_block->l[(i+8)&15] ^ _block->l[(i+2)&15] ^ _block->l[i&15],1))

#define S_R0(v,w,x,y,z,i) {z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5);w=ROL32(w,30);}
#define S_R1(v,w,x,y,z,i) {z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5);w=ROL32(w,30);}
#define S_R2(v,w,x,y,z,i) {z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5);w=ROL32(w,30);}
#define S_R3(v,w,x,y,z,i) {z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5);w=ROL32(w,30);}
#define S_R4(v,w,x,y,z,i) {z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5);w=ROL32(w,30);}

CSHA1::CSHA1()
{
}

void CSHA1::Generator::init()	
{
	_state[0] = 0x67452301;
	_state[1] = 0xEFCDAB89;
	_state[2] = 0x98BADCFE;
	_state[3] = 0x10325476;
	_state[4] = 0xC3D2E1F0;

	_count[0] = 0;
	_count[1] = 0;
}

void CSHA1::Generator::update(const void* data, uint32 length)
{
	uint32 j = ((_count[0] >> 3) & 0x3F);

	if((_count[0] += (length << 3)) < (length << 3))
		++_count[1];

	_count[1] += (length >> 29);

	uint32 i;
	if((j + length) > 63)
	{
		i = 64 - j;
		memcpy(&_buffer[j], data, i);
		transform(_state, _buffer);

		for( ; (i + 63) < length; i += 64)
			transform(_state, &((const uint8*) data)[i]);

		j = 0;
	}
	else i = 0;

	if((length - i) != 0)
		memcpy(&_buffer[j], &((const uint8*) data)[i], length - i);
}

void CSHA1::Generator::done()
{
	uint32 i;

	uint8 finalCount[8];
	for(i = 0; i < 8; ++i)
		finalCount[i] = static_cast<uint8>((_count[((i >= 4) ? 0 : 1)] >>
			((3 - (i & 3)) * 8) ) & 0xFF);

	update((uint8*)"\200", 1);

	while((_count[0] & 504) != 448)
		update((uint8*)"\0", 1);

	update(finalCount, 8);

	for(i = 0; i < 20; ++i)
		_digest[i] = static_cast<uint8>((_state[i >> 2] >> ((3 -
			(i & 3)) * 8)) & 0xFF);

	memset(_buffer, 0, 64);
	memset(_state, 0, 20);
	memset(_count, 0, 8);
	memset(finalCount, 0, 8);
	transform(_state, _buffer);
}

CSHA1 CSHA1::Generator::get() const
{
	CSHA1 out;
	memcpy(out._digest, _digest, 20);
	return out;
}

void CSHA1::Generator::transform(uint32* state, const uint8* buffer)
{
	uint32 a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];

	memcpy(_block, buffer, 64);

	S_R0(a,b,c,d,e, 0); S_R0(e,a,b,c,d, 1); S_R0(d,e,a,b,c, 2); S_R0(c,d,e,a,b, 3);
	S_R0(b,c,d,e,a, 4); S_R0(a,b,c,d,e, 5); S_R0(e,a,b,c,d, 6); S_R0(d,e,a,b,c, 7);
	S_R0(c,d,e,a,b, 8); S_R0(b,c,d,e,a, 9); S_R0(a,b,c,d,e,10); S_R0(e,a,b,c,d,11);
	S_R0(d,e,a,b,c,12); S_R0(c,d,e,a,b,13); S_R0(b,c,d,e,a,14); S_R0(a,b,c,d,e,15);
	S_R1(e,a,b,c,d,16); S_R1(d,e,a,b,c,17); S_R1(c,d,e,a,b,18); S_R1(b,c,d,e,a,19);
	S_R2(a,b,c,d,e,20); S_R2(e,a,b,c,d,21); S_R2(d,e,a,b,c,22); S_R2(c,d,e,a,b,23);
	S_R2(b,c,d,e,a,24); S_R2(a,b,c,d,e,25); S_R2(e,a,b,c,d,26); S_R2(d,e,a,b,c,27);
	S_R2(c,d,e,a,b,28); S_R2(b,c,d,e,a,29); S_R2(a,b,c,d,e,30); S_R2(e,a,b,c,d,31);
	S_R2(d,e,a,b,c,32); S_R2(c,d,e,a,b,33); S_R2(b,c,d,e,a,34); S_R2(a,b,c,d,e,35);
	S_R2(e,a,b,c,d,36); S_R2(d,e,a,b,c,37); S_R2(c,d,e,a,b,38); S_R2(b,c,d,e,a,39);
	S_R3(a,b,c,d,e,40); S_R3(e,a,b,c,d,41); S_R3(d,e,a,b,c,42); S_R3(c,d,e,a,b,43);
	S_R3(b,c,d,e,a,44); S_R3(a,b,c,d,e,45); S_R3(e,a,b,c,d,46); S_R3(d,e,a,b,c,47);
	S_R3(c,d,e,a,b,48); S_R3(b,c,d,e,a,49); S_R3(a,b,c,d,e,50); S_R3(e,a,b,c,d,51);
	S_R3(d,e,a,b,c,52); S_R3(c,d,e,a,b,53); S_R3(b,c,d,e,a,54); S_R3(a,b,c,d,e,55);
	S_R3(e,a,b,c,d,56); S_R3(d,e,a,b,c,57); S_R3(c,d,e,a,b,58); S_R3(b,c,d,e,a,59);
	S_R4(a,b,c,d,e,60); S_R4(e,a,b,c,d,61); S_R4(d,e,a,b,c,62); S_R4(c,d,e,a,b,63);
	S_R4(b,c,d,e,a,64); S_R4(a,b,c,d,e,65); S_R4(e,a,b,c,d,66); S_R4(d,e,a,b,c,67);
	S_R4(c,d,e,a,b,68); S_R4(b,c,d,e,a,69); S_R4(a,b,c,d,e,70); S_R4(e,a,b,c,d,71);
	S_R4(d,e,a,b,c,72); S_R4(c,d,e,a,b,73); S_R4(b,c,d,e,a,74); S_R4(a,b,c,d,e,75);
	S_R4(e,a,b,c,d,76); S_R4(d,e,a,b,c,77); S_R4(c,d,e,a,b,78); S_R4(b,c,d,e,a,79);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	a = b = c = d = e = 0;
}

void CSHA1::getDigest(uint8 digest[20]) const
{
	memcpy(digest, _digest, 20);
}

CString CSHA1::tostring() const
{
	char buf[2 * 20+1];
	buf[20] = 0;

	static const char* hex = "0123456789abcdef";
	for ( uint32 i=0; i<20; ++i )
	{
		buf[i*2] = hex[(_digest[i] >> 4) & 0xF];
		buf[i*2+1] = hex[_digest[i] & 0xF];
	}

	return CString(buf);
}
