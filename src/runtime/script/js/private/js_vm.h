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
js_vm.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CJavascriptVM : public IVMHost, public enable_shared_from_this<CJavascriptVM>
{

public:
	virtual ELanguage getLanguage() override;
	virtual bool init() override;
	virtual void shutdown() override;

	virtual bool isRunning() override;

	virtual weak_ptr<IVMClass> loadGameVMClass(shared_ptr<CPackage> gamePackage) override;
	virtual bool includeGameScript() override;
	virtual bool validateGameScript() override;

	v8::Isolate* getIsolate();
	v8::Local<v8::ObjectTemplate> getGlobalTemplate();

private:
	void createGlobalTemplate();

	v8::Eternal<v8::ObjectTemplate> _globalTemplate;
	std::unique_ptr<v8::Platform> _platform;
	v8::ArrayBuffer::Allocator* _allocator;
	v8::Isolate* _isolate;
	map<CGUID, shared_ptr<class CJavascriptVMClass>> _loadedClasses;
};

extern void testJavascript();

}