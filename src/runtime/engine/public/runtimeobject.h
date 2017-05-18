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

//Ideally, this shouldn't be exported, it's just quicker than making interfaces for everything
class METACADE_API CRuntimeObject
{
public:
	CRuntimeObject(class IRuntime* runtime);
	CRuntimeObject(CRuntimeObject* outer);

protected:

#ifdef ENGINE_PRIVATE
	template<typename T, typename... ArgT> T *construct(ArgT&&... args)
	{
		return new(zalloc(sizeof(T))) T(args...);
	}

	template<typename T>
	void destroy(T* obj)
	{
		if ( obj == nullptr ) return;
		obj->~T();
		zfree(obj);
	}

	//not great, but only compiles when you use it, so whatever I'll fix it later
	template <typename T, typename... ArgT> shared_ptr<T> makeShared(ArgT&&... args)
	{
		return shared_ptr<T>
			( this->construct<T>( args... )
			, [this](T* ptr) { this->destroy(ptr); });
	}

	template<typename T>
	T* castAsset(const CAssetRef& ref) 
	{ 
		IAsset* asset = ref.get(_runtime); 
		if (!asset || !((T*)(asset))->checkType()) return nullptr; 
		return (T*)asset; 
	}

	void log(EMessageType type, const char* message, ...);
	void* zalloc(uint32 size);
	void* zrealloc(void* pointer, uint32 size);
	void zfree(void* pointer);
	void zfree(const void* pointer);

	class IFileObject* openFile(const CString& name, EFileIOMode mode);
	void closeFIle(class IFileObject* file);

	bool listFilesInDirectory(class IFileCollection* collection, const char* dir, const char* extFilter = nullptr);

	class IRuntime* getRuntime() const;

	class CIndex allocateImageIndex();

	class IVMHost* getLuaVM();
#endif

private:
	class IRuntime* _runtime;
};

}