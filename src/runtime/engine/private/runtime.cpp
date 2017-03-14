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
runtime.cpp:
===============================================================================
*/

#include "engine_private.h"

class CDefaultAllocator : public IAllocator
{
public:
	virtual void* memrealloc(void* pointer, uint32 size) override
	{
		return realloc(pointer, size);
	}

	virtual void memfree(void* pointer) override
	{
		free(pointer);
	}
};

static CDefaultAllocator gDefaultAllocator;

CRuntime::CRuntime()
	: CRuntimeObject(this)
	, _packageManager(nullptr)
	, _runtimeEnvironment(nullptr)
	, _renderTest(make_shared<CRenderTest>(this))
{

}

CRuntime::~CRuntime()
{

}

bool CRuntime::initialize(IRuntimeEnvironment* env)
{
	_runtimeEnvironment = env;
	if ( _runtimeEnvironment == nullptr ) return false;

	_packageManager = make_shared<CPackageManager>(_runtimeEnvironment->getFileSystem());
	_renderTest = make_shared<CRenderTest>(this);

	if ( !_renderTest->init() ) return false;

	filesystemTest();

	return true;
}

IPackageManager* CRuntime::getPackageManager()
{
	return _packageManager.get();
}

IRenderTest* CRuntime::getRenderTest()
{
	return _renderTest.get();
}

IAllocator* CRuntime::getAllocator()
{
	IAllocator *impl = _runtimeEnvironment->getAllocator();
	if ( impl == nullptr ) impl = &gDefaultAllocator;

	return impl;
}

IFileSystem* CRuntime::getFilesystem()
{
	return _runtimeEnvironment->getFileSystem();
}

ILogger* CRuntime::getLogger()
{
	return _runtimeEnvironment->getLogger();
}

bool CRuntime::filesystemTest()
{
	IFileSystem* fs = _runtimeEnvironment->getFileSystem();
	if ( fs == nullptr )
	{
		log(LOG_WARN, "No Filesystem Provided");
		return false;
	}

	string writeTestString("Hello World");

	{
		IFileObject* obj = fs->openFile("TestFile.txt", FILE_WRITE);
		if ( obj == nullptr )
		{
			log(LOG_ERROR, "Failed to create test file");
			return false;
		}

		
		if ( !obj->write((void*)writeTestString.c_str(), writeTestString.length()) )
		{
			log(LOG_ERROR, "Error while writing test data");
			return false;
		}

		fs->closeFile(obj);
	}

	{
		IFileObject* obj = fs->openFile("TestFile.txt", FILE_READ);
		if ( obj == nullptr )
		{
			log(LOG_ERROR, "Failed to re-open test file");
			return false;
		}

		uint32 size = obj->getSize();
		if ( size == 0 )
		{
			log(LOG_ERROR, "Test file was empty");
			return false;
		}

		char* buffer = new char[size+1];
		if ( !obj->read(buffer, size) )
		{
			log(LOG_ERROR, "Failed to read file contents");
			delete [] buffer;
			return false;	
		}
		buffer[size] = 0;

		string str(buffer);
		if ( str != writeTestString )
		{
			log(LOG_ERROR, "Parity failed: %s != %s", str.c_str(), writeTestString.c_str());
			delete [] buffer;
			return false;
		}

		delete [] buffer;
	}

	log(LOG_MESSAGE, "All tests passed");
	return true;
}
