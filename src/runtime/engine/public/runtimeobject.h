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
runtimemanaged.h: Localizes runtime functionality to object
===============================================================================
*/

#pragma once

namespace Arcade
{

class METACADE_API CRuntimeObject
{
public:
	CRuntimeObject(class IRuntime* runtime);
	CRuntimeObject(CRuntimeObject* outer);

protected:
	void log(EMessageType type, const char* message, ...);
	void* alloc(unsigned int size);
	void* realloc(void* pointer, unsigned int size);
	void free(void* pointer);
	void free(const void* pointer);

	class IFileObject* openFile(const CString& name, EFileIOMode mode);
	void closeFIle(class IFileObject* file);

	bool listFilesInDirectory(class IFileCollection* collection, const char* dir, const char* extFilter = nullptr);

private:
	class IRuntime* _runtime;
};

}