#pragma once

#include "metacade_amal.h"

using namespace Arcade;

#include <memory>

using std::shared_ptr;

class FileSystem : public IFileSystem
{

public:
	virtual IFileObject* openFile(const CString& filename, EFileIOMode mode) override;
	virtual void closeFile(IFileObject* file) override;
	virtual bool listFilesInDirectory(IFileCollection* collection, const char* dir, const char* extFilter = nullptr) override;
};

class Logger : public ILogger
{
public:
	virtual void log(const char* text, EMessageType type) override;
};

class NativeEnv : public IRuntimeEnvironment
{
public:
	NativeEnv();

	virtual class IAllocator* getAllocator() override;
	virtual class IFileSystem* getFileSystem() override;
	virtual class ILogger* getLogger() override;
	virtual class IRenderer* getRenderer() override;

	void setRenderer(shared_ptr<IRenderer> renderer);

	int64 getMemUsage() const;

private:
	shared_ptr<FileSystem> _fs;
	shared_ptr<Logger> _logger;
	shared_ptr<IRenderer> _renderer;
};