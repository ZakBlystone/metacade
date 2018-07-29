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
#include "sound_private.h"
#include "script/lua/lua_private.h"

#if WITH_V8
#include "script/js/js_private.h"
#endif

#include <stdarg.h>

struct CRefTest
{
	CReferenceCounter counter;
};

CRuntime::CRuntime()
	: _packageManager(nullptr)
	, _runtimeEnvironment(nullptr)
{
}

CRuntime::~CRuntime()
{
	std::cout << "Destruct RUNTIME" << std::endl;
	if ( isCurrent() ) 
	{
		_packageManager.reset();
		_codeVM[LANG_JAVASCRIPT].reset();
		_codeVM[LANG_LUA].reset();
		gRuntime = nullptr;
		gAllocator = CDefaultAllocator::get();
	}
}

bool CRuntime::initialize(IRuntimeEnvironment* env)
{
	_runtimeEnvironment = env;
	if ( _runtimeEnvironment == nullptr ) return false;

	makeCurrent();

	//testJavascript();

	_packageManager = makeShared<CPackageManager>();
	_textureIndices = makeShared<CIndexAllocator>();

#if WITH_V8
	_codeVM[LANG_JAVASCRIPT] = makeShared<CJavascriptVM>();
	if ( !_codeVM[LANG_JAVASCRIPT]->init() ) return false;
#endif

	_codeVM[LANG_LUA] = makeShared<CLuaVM>();
	if ( !_codeVM[LANG_LUA]->init() ) return false;

	//_renderTest = makeShared<CRenderTest>(this);

	//if ( !_renderTest->init() ) return false;

	//filesystemTest();

	/*CSHA1 sha;
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
	
	log(LOG_MESSAGE, "=REF: %i", A.counter.count());*/

	return true;
}

IPackageManager* CRuntime::getPackageManager()
{
	return _packageManager.get();
}

IAllocator* CRuntime::getAllocator() const
{
	IAllocator *impl = _runtimeEnvironment->getAllocator();
	if ( impl == nullptr ) impl = CDefaultAllocator::get();

	return impl;
}

IFileSystem* CRuntime::getFilesystem() const
{
	return _runtimeEnvironment->getFileSystem();
}

ILogger* CRuntime::getLogger() const
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
		CFileHandle obj("TestFile.txt", FILE_WRITE);
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
		CFileHandle obj("TestFile.txt", FILE_READ);
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

		char* buffer = (char*) zalloc(size+1);
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

		zfree( buffer );
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

void CRuntime::makeCurrent()
{
	gRuntime = this;
	gAllocator = getAllocator();
}

bool CRuntime::isCurrent() const
{
	return gRuntime == this;
}

IMetaData* CRuntime::createMetaData()
{
	return construct<CMetaData>();
}

void CRuntime::deleteMetaData(IMetaData* data)
{
	destroy<CMetaData>((CMetaData*)data);
}

ISoundMixer* CRuntime::createSoundMixer(CMixerSettings settings)
{
	return construct<CSoundMixer>(settings);
}

void CRuntime::deleteSoundMixer(ISoundMixer* mixer)
{
	return destroy<CSoundMixer>((CSoundMixer*)mixer);
}

IGameClass* CRuntime::getGameClassForPackage(IPackage* package)
{
	if ( package == nullptr ) 
	{
		log(LOG_ERROR, "Null package");
		return nullptr;
	}

	CGUID packageID = package->getPackageID();

	auto existing = _classes.find(packageID);
	if ( existing != _classes.end() )
	{
		return (*existing).second.get();
	}

	weak_ptr<CPackage> pkg = _packageManager->getSharedPackageByID(packageID);
	if ( pkg.expired() ) return nullptr;

	class CEnableGameClass : public CGameClass
	{
	public:
		CEnableGameClass(weak_ptr<CPackage> package)
			: CGameClass(package) {}
	};

	shared_ptr<CEnableGameClass> newClass = makeShared<CEnableGameClass>(pkg);

	_classes.insert(make_pair(packageID, newClass));

	return newClass.get();
}

Arcade::IVMHost* Arcade::CRuntime::getCodeVM( ELanguage language )
{
	return _codeVM[language].get();
}

thread_local CRuntime* Arcade::gRuntime = nullptr;

//from CRuntimeObject

void Arcade::log(EMessageType type, const char* message, ...)
{
	if ( gRuntime == nullptr ) return;

	va_list args;

	va_start(args, message);
	int32 len = _vscprintf(message, args) + 1;

	char* buffer = (char*)zalloc(len);
	vsprintf_s(buffer, len, message, args);

	ILogger* logger = gRuntime->getLogger();
	logger->log(buffer, type);

	zfree(buffer);
	va_end(args);
}

IFileObject* Arcade::openFile(const CString& name, EFileIOMode mode)
{
	if ( gRuntime == nullptr ) return nullptr;
	IFileSystem* fs = gRuntime->getFilesystem();

	if (fs == nullptr) return nullptr;
	return fs->openFile(name, mode);
}

void Arcade::closeFIle(IFileObject* file)
{
	if ( gRuntime == nullptr ) return;
	IFileSystem* fs = gRuntime->getFilesystem();

	if (fs == nullptr) return;
	fs->closeFile(file);
}

bool Arcade::listFilesInDirectory(IFileCollection* collection, const char* dir, const char* extFilter)
{
	if ( gRuntime == nullptr ) return nullptr;
	IFileSystem* fs = gRuntime->getFilesystem();

	if (fs == nullptr) return false;
	return fs->listFilesInDirectory(collection, dir, extFilter);
}