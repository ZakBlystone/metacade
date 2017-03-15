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
scriptresource.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CCodeAsset : public CAsset<ASSET_CODE>
{
public:
	CCodeAsset(CRuntimeObject* outer);

	virtual bool load(IFileObject* file) override;
	virtual bool save(IFileObject* file) override;
	virtual bool validate() const override;
	virtual void release() override;

	const char* getCodeBuffer() const;
	uint32 getCodeLength() const;

	void setCodeBuffer(const char* buffer, uint32 size);

	char* _code;
	uint32 _codeLength;
};

}