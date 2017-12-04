#include "native.h"
#include <windows.h>

#include <string>
#include <stdio.h>
#include <iostream>

using std::string;
using std::make_shared;

class FileObject : public IFileObject
{
public:
	virtual ~FileObject();
	virtual bool seek(uint32 Offset);
	virtual bool read(void *Data, uint32 Bytes);
	virtual bool write(const void *Data, uint32 Bytes);
	virtual uint32 tell();
	virtual uint32 getSize();

	FILE *FP;
	unsigned int Size;
	CString _filename;
};

FileObject::~FileObject()
{
	if (FP) { fflush(FP); fclose(FP); }
}

bool FileObject::seek(uint32 Offset)
{
	return fseek(FP, Offset, SEEK_SET) == 0;
}

bool FileObject::read(void *Data, uint32 Bytes)
{
	return fread(Data, 1, Bytes, FP) == Bytes;
}

bool FileObject::write(const void *Data, uint32 Bytes)
{
	return fwrite(Data, 1, Bytes, FP) == Bytes;
}

uint32 FileObject::tell()
{
	return ftell(FP);
}

uint32 FileObject::getSize()
{
	return Size;
}

Arcade::IFileObject* FileSystem::openFile(const CString& filename, EFileIOMode mode)
{
	FileObject *F = new FileObject;
	fopen_s(&F->FP, *filename, mode == FILE_READ ? "rb" : "wb");

	if (!F->FP) { delete F; return nullptr; }
	F->Size = 0;
	F->_filename = filename;

	if (mode == FILE_READ)
	{
		fseek(F->FP, 0, SEEK_END);
		F->Size = ftell(F->FP);
		fseek(F->FP, 0, SEEK_SET);
	}

	std::cout << "OPEN: " << *filename << std::endl;

	return F;
}

void FileSystem::closeFile(IFileObject* file)
{
	FileObject *F = (FileObject *)file;
	if (!F) return;

	std::cout << "CLOSE: " << *F->_filename << std::endl;

	delete F;
}

bool FileSystem::listFilesInDirectory(IFileCollection* collection, const char* dir, const char* extFilter /*= nullptr*/)
{
	string path(dir);
	path += "*";

	std::cout << "SEARCH: " << path << std::endl;

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(path.c_str(), &f);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				string fn = string(dir) + string(f.cFileName);
				if ( extFilter != nullptr )
				{
					string ext(extFilter);
					if ( fn.substr(fn.size()-ext.size(), ext.size()) != ext )
					{
						continue;
					}
				}
				
				collection->add(fn.c_str());
			}
		} while (FindNextFile(h, &f));
	}
	else
	{
		return false;
	}
	return true;
}

NativeEnv::NativeEnv() 
	: _fs(make_shared<FileSystem>())
	, _logger(make_shared<Logger>())
{

}

class CTestAllocator : public IAllocator
{
public:
	int64 _bytesAllocated;

	CTestAllocator()
		: _bytesAllocated(0)
	{}

	virtual void* memrealloc(void* pointer, uint32 size) override
	{
		size += sizeof(uint32);

		uint8* mem = (uint8*) realloc(pointer, size);
		*(uint32*)(mem) = size;

		_bytesAllocated += size;
		//std::cout << "A: " << _bytesAllocated << " +" << size << std::endl;
		return mem + sizeof(uint32);
	}

	virtual void memfree(void* pointer) override
	{
		uint8* mem = ((uint8*) (pointer)) - sizeof(uint32);
		_bytesAllocated -= *(uint32*)(mem);

		//std::cout << "F: " << _bytesAllocated << " -" << *(uint32*)(mem) << std::endl;

		free(mem);
	}
};

static CTestAllocator gTestAllocator;

class IAllocator* NativeEnv::getAllocator()
{
	return &gTestAllocator;
}

class IFileSystem* NativeEnv::getFileSystem()
{
	return _fs.get();
}

class ILogger* NativeEnv::getLogger()
{
	return _logger.get();
}

int64 NativeEnv::getMemUsage() const
{
	return gTestAllocator._bytesAllocated;
}

void Logger::log(const char* text, EMessageType type)
{
	static const char* logTypes[EMessageType::LOG_MAX] = {
		"MESSAGE",
		"WARNING",
		"ERROR",
	};

	std::cout << "[" << logTypes[type] << "]: " << text << std::endl;
}
