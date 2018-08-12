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

namespace Arcade
{

class CRuntime : public IRuntime
{
public:
	CRuntime();
	~CRuntime();

	virtual bool initialize(IRuntimeEnvironment* env);
	virtual IPackageManager* getPackageManager();

	virtual void makeCurrent();
	virtual bool isCurrent() const;

	virtual IMetaData* createMetaData();
	virtual void deleteMetaData(IMetaData* data);

	virtual ISoundMixer* createSoundMixer(CMixerSettings settings);
	virtual void deleteSoundMixer(ISoundMixer* mixer);

	virtual IGameClass* getGameClassForPackage(IPackage* package);

	IAllocator* getAllocator() const;
	IFileSystem* getFilesystem() const;
	ILogger* getLogger() const;
	IRenderer* getRenderer() const;

	bool filesystemTest();

	shared_ptr<CIndexAllocator> getImageIndexAllocator();

	IVMHost* getCodeVM( ELanguage language );

private:
	shared_ptr<CPackageManager> _packageManager;
	IRuntimeEnvironment* _runtimeEnvironment;
	shared_ptr<CIndexAllocator> _textureIndices;
	shared_ptr<class IVMHost> _codeVM[ ELanguage::LANG_NUM ];

	map<CGUID, shared_ptr<class CGameClass>> _classes;
};

extern thread_local CRuntime* gRuntime;

extern void log(EMessageType type, const char* message, ...);

extern class IFileObject* openFile(const CString& name, EFileIOMode mode);
extern void closeFIle(class IFileObject* file);

extern bool listFilesInDirectory(class IFileCollection* collection, const char* dir, const char* extFilter = nullptr);

}