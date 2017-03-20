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
#include "script/lua/lua_private.h"

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


struct CRefTest
{
	CReferenceCounter counter;
};

CRuntime::CRuntime()
	: CRuntimeObject((IRuntime*) this)
	, _packageManager(nullptr)
	, _runtimeEnvironment(nullptr)
	, _renderTest(make_shared<CRenderTest>(this))
	, _textureIndices(make_shared<CIndexAllocator>(this))
	, _luaVM(make_shared<CLuaVM>())
{
}

CRuntime::~CRuntime()
{

}

bool CRuntime::initialize(IRuntimeEnvironment* env)
{
	_runtimeEnvironment = env;
	if ( _runtimeEnvironment == nullptr ) return false;
	if ( !_luaVM->init() ) return false;

	_packageManager = make_shared<CPackageManager>(this);
	_renderTest = make_shared<CRenderTest>(this);

	if ( !_renderTest->init() ) return false;

	filesystemTest();

	CSHA1 sha;
	CSHA1::Generator gen;
	gen.init();
	gen.update("",0);
	gen.done();

	sha = gen.get();

	log(LOG_MESSAGE, "SHA:%s", *sha.tostring());

	log(LOG_MESSAGE, "AbCdEf: %s %s", *CString("AbCdEf").lower(), *CString("AbCdEf").upper());

	CRefTest A;
	log(LOG_MESSAGE, "=REF: %i", A.counter.count());

	{
		CRefTest B(A);
		log(LOG_MESSAGE, "=REF: %i", A.counter.count());

		{
			CRefTest C = B;
			log(LOG_MESSAGE, "=REF: %i", A.counter.count());
		}

		log(LOG_MESSAGE, "=REF: %i", A.counter.count());
	}
	
	log(LOG_MESSAGE, "=REF: %i", A.counter.count());

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
		CFileHandle obj("TestFile.txt", FILE_WRITE, this);
		if ( !obj.isValid() )
		{
			log(LOG_ERROR, "Failed to create test file");
			return false;
		}
		
		if ( !obj->write((void*)writeTestString.c_str(), (uint32)writeTestString.length()) )
		{
			log(LOG_ERROR, "Error while writing test data");
			return false;
		}
	}

	{
		CFileHandle obj("TestFile.txt", FILE_READ, this);
		if ( !obj.isValid() )
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

	{
		CFileCollection fc;
		fs->listFilesInDirectory(&fc, "./", "mpkg");

		for ( uint32 i=0; i<fc.numFiles(); ++i )
		{
			log(LOG_MESSAGE, "FILE[%i]: %s", i, *fc.getFile(i));
		}
	}

	log(LOG_MESSAGE, "All tests passed");
	return true;
}

shared_ptr<CIndexAllocator> CRuntime::getImageIndexAllocator()
{
	return _textureIndices;
}

Arcade::IRenderTest* Arcade::CRuntime::createRenderTest()
{
	CRenderTest* test = new CRenderTest(this);
	if ( !test->init() ) return nullptr;

	return test;
}

void Arcade::CRuntime::deleteRenderTest(IRenderTest* test)
{
	delete test;
}

Arcade::IVMHost* Arcade::CRuntime::getLuaVM()
{
	return _luaVM.get();
}
