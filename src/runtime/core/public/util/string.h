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
string.h: Replaces STL string, can be exported out of API
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CString
{
public:
	CString();
	CString(const CString& other);
	CString(const char* str);
	~CString();

	uint32 length() const;
	bool empty() const;
	bool startsWith(const CString& string) const;
	bool endsWith(const CString& string) const;
	bool contains(const CString& string) const;

	//returns -1 if not found
	int32 find(const CString& string, uint32 start = 0) const;

	CString chopLeft(uint32 len) const;
	CString chopRight(uint32 len) const;
	CString sub(uint32 offset, uint32 len) const;
	CString lower() const;
	CString upper() const;

	CString operator/(const CString& other) const;
	CString operator/(const char* other) const;
	CString operator+(const CString& other) const;
	CString operator+(const char* other) const;
	CString &operator=(const CString& other);
	bool operator==(const CString& other) const;
	bool operator!=(const CString& other) const;
	bool operator<(const CString& other) const;
	const char* operator*() const;

	const char* get() const;

private:
	CString(uint32 length);

	void reset();
	

	char* _string;
	uint32 _length;
	uint32* _refs;
};

}