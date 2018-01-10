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
variant.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

enum EVariantType
{
	VT_NONE,
	VT_BOOLEAN,
	VT_UINT,
	VT_INT,
	VT_DOUBLE,
	VT_STRING,
	VT_MAX,
};

class METACADE_API CVariant
{
public:
	template<typename T>
	CVariant(const T& value)
		: _type(VT_NONE) 
		, _strdata(nullptr)
		, _strrefs(nullptr)
	{
		set(value);
	}

	CVariant();
	CVariant(const CVariant& other);
	~CVariant();

	bool isSet() const;

	void set(bool value);
	void set(uint64 value);
	void set(int64 value);
	void set(uint32 value) { set((uint64) value); }
	void set(int32 value) { set((int64) value); }
	void set(uint16 value) { set((uint64) value); }
	void set(int16 value) { set((int64) value); }
	void set(uint8 value) { set((uint64) value); }
	void set(int8 value) { set((int64) value); }
	void set(double value);
	void set(float value);
	void set(const char *value);
	void set(const class CString& str);

	EVariantType type() const;
	const char *getTypeName() const;

	bool get(bool& value) const;
	bool get(uint64& value) const;
	bool get(int64& value) const;
	bool get(uint32& value) const { uint64 v; bool s = get(v); if(s) value = (uint32) v; return s; }
	bool get(int32& value) const { int64 v; bool s = get(v); if(s) value = (int32) v; return s; }
	bool get(uint16& value) const { uint64 v; bool s = get(v); if(s) value = (uint16) v; return s; }
	bool get(int16& value) const { int64 v; bool s = get(v); if(s) value = (int16) v; return s; }
	bool get(uint8& value) const { uint64 v; bool s = get(v); if(s) value = (uint8) v; return s; }
	bool get(int8& value) const { int64 v; bool s = get(v); if(s) value = (int8) v; return s; }
	bool get(double& value) const;
	bool get(const char*& buffer, int32& length) const;
	bool get(class CString& str) const;

	CVariant& operator = (const CVariant& other);

	template<typename T>
	CVariant& operator = (const T& value)
	{
		set(value);
		return *this;
	}

	CString toString() const;

private:

	void reset();

	EVariantType _type;
	uint8 _pod[8];
	uint8 *_strdata;
	uint32 *_strrefs;
};

}