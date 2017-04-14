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
functioncall.cpp: call VM functions externally
===============================================================================
*/

#include "engine_private.h"

CFunctionCall::CFunctionCall(const CString& func)
	: _func(func)
	, _args(new vector<CVariant>())
{

}

Arcade::CFunctionCall::CFunctionCall()
	: _func("null")
	, _args(new vector<CVariant>())
{

}

void CFunctionCall::setFunction(const CString& func)
{
	_func = func;
}

CFunctionCall::~CFunctionCall()
{
	if (_counter.unique()) delete ((vector<CVariant>*)(_args));
}

uint32 CFunctionCall::numArgs() const
{
	return (uint32) ((vector<CVariant>*)(_args))->size();
}

CVariant CFunctionCall::getArg(uint32 i) const
{
	return (*((vector<CVariant>*)(_args)))[i];
}

CString CFunctionCall::getFunction() const
{
	return _func;
}

void CFunctionCall::addArg(const CVariant& v)
{
	((vector<CVariant>*)(_args))->push_back(v);
}
