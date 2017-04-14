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
functioncall.h: call VM functions externally
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CFunctionCall
{
public:
	CFunctionCall();
	CFunctionCall(const CString& func);
	~CFunctionCall();

	template<typename...Args>
	CFunctionCall(const CString& func, const Args&... args)
		: CFunctionCall(func)
	{
		int32 length = sizeof...(args);
		CVariant vals[] = {args...};
		for ( int32 i=0; i<length; ++i )
			addArg(vals[i]);
	}

	uint32 numArgs() const;
	CVariant getArg(uint32 i) const;

	CString getFunction() const;
	void setFunction(const CString& func);

	void addArg(const CVariant& v);

private:

	CString _func;
	CReferenceCounter _counter;
	void* _args;
};

}