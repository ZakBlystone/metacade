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
metadata.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CMetaData : public IMetaData
{
public:
	virtual uint32 getNumKeys() const;
	virtual CString getKey(uint32 i) const;
	virtual CString getValue(uint32 i) const;
	virtual CString getValue(const CString& key) const;

	virtual void setKeyValuePair(const CString& key, const CString& value);

	bool save(IFileObject* file) const;
	bool load(IFileObject* file);

private:
	map<CString, CString> _keyvalues;
	vector<CString> _keys;
};

}