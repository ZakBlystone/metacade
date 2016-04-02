/*
* <FileInterface.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#pragma once

namespace Arcade
{
	//
	//IO modes to supply when opening a file
	//
	enum FileIOMode
	{
		FILE_READ,
		FILE_WRITE,
	};

	//
	//Interface for an open file
	//
	class IFileObject
	{
	public:
		virtual bool Seek(unsigned int Offset) = 0;					//Move file pointer to this byte in the file
		virtual bool Read(void *Data, unsigned int Bytes) = 0;		//Read 'Bytes' of 'Data' from the file
		virtual bool Write(void *Data, unsigned int Bytes) = 0;		//Write 'Bytes' of 'Data' to the file
		virtual unsigned int Tell() = 0;							//Get the position of the file pointer
		virtual unsigned int GetSize() = 0;							//Get the size of the file
	};

	//
	//Interface for opening and closing IFileObjects
	//
	class IHostFileInterface
	{
	public:
		virtual IFileObject *OpenFile(const char *FileName, FileIOMode Mode) = 0;
		virtual void CloseFile(IFileObject *Object) = 0;
	};

}