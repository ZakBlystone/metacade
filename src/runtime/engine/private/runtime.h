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
runtime.h:
===============================================================================
*/

#pragma once

#include "metacade_types.h"

#include "rendertest.h"

namespace Arcade
{

class CRuntime : public IRuntime, public CRuntimeObject
{
public:
	CRuntime();
	virtual ~CRuntime();

	virtual bool initialize(IRuntimeEnvironment* env);
	virtual IPackageManager* getPackageManager();
	virtual IRenderTest* getRenderTest();

	IAllocator* getAllocator();
	IFileSystem* getFilesystem();
	ILogger* getLogger();

	bool filesystemTest();

	shared_ptr<CIndexAllocator> getImageIndexAllocator();

private:
	shared_ptr<CPackageManager> _packageManager;
	shared_ptr<CRenderTest> _renderTest;
	IRuntimeEnvironment* _runtimeEnvironment;
	shared_ptr<CIndexAllocator> _textureIndices;
};

}