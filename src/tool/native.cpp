#include "native.h"
#include <windows.h>

#include <string>
#include <stdio.h>

using std::string;
using std::make_shared;

class FileObject : public IFileObject
{
public:
	virtual ~FileObject();
	virtual bool seek(uint32 Offset);
	virtual bool read(void *Data, uint32 Bytes);
	virtual bool write(void *Data, uint32 Bytes);
	virtual uint32 tell();
	virtual uint32 getSize();

	FILE *FP;
	unsigned int Size;
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

bool FileObject::write(void *Data, uint32 Bytes)
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

Arcade::IFileObject* FileSystem::openFile(const char* filename, EFileIOMode mode)
{
	FileObject *F = new FileObject;
	F->FP = fopen(filename, mode == FILE_READ ? "rb" : "wb");

	if (!F->FP) { delete F; return nullptr; }
	F->Size = 0;

	if (mode == FILE_READ)
	{
		fseek(F->FP, 0, SEEK_END);
		F->Size = ftell(F->FP);
		fseek(F->FP, 0, SEEK_SET);
	}

	return F;
}

void FileSystem::closeFile(IFileObject* file)
{
	FileObject *F = (FileObject *)file;
	if (!F) return;
	delete F;
}

bool FileSystem::listFilesInDirectory(void(*callback) (const char*), const char* dir, const char* extFilter /*= nullptr*/)
{
	string path(dir);
	path += "*";

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(path.c_str(), &f);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				string fn = path + string(f.cFileName);
				callback(fn.c_str());
			}
		} while (FindNextFile(h, &f));
	}
	else
	{
		return false;
	}
	return true;
}

NativeEnv::NativeEnv() : _fs(make_shared<FileSystem>())
{

}

class IAllocator* NativeEnv::getAllocator()
{
	return nullptr;
}

class IFileSystem* NativeEnv::getFileSystem()
{
	return _fs.get();
}
