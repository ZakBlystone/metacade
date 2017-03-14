#pragma once

#include "metacade_amal.h"

using namespace Arcade;

#include <memory>

using std::shared_ptr;

class FileSystem : public IFileSystem
{

public:
	virtual IFileObject* openFile(const char* filename, EFileIOMode mode) override;
	virtual void closeFile(IFileObject* file) override;
	virtual bool listFilesInDirectory(void(*callback) (const char*), const char* dir, const char* extFilter = nullptr) override;
};

class NativeEnv : public IRuntimeEnvironment
{
public:
	NativeEnv();

	virtual class IAllocator* getAllocator() override;
	virtual class IFileSystem* getFileSystem() override;

private:
	shared_ptr<FileSystem> _fs;
};