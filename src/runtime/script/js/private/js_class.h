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
js_class.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CJavascriptVMClass : public IVMClass, public enable_shared_from_this<CJavascriptVMClass>
{
public:
	CJavascriptVMClass(weak_ptr<class CJavascriptVM> host);

	virtual bool reload() override;
	virtual class IVMHost* getHost() override;
	virtual shared_ptr<class CMetaData> getMetaData() override;
	virtual shared_ptr<class IVMInstance> createVMInstance() override;

	bool loadFromPackage(weak_ptr<CPackage> package);

	v8::Local<v8::Context> getContext();
	v8::Isolate* getIsolate();

private:
	void createGlobals(v8::Local<v8::Context>& context, v8::Local<v8::Object>& global);

	weak_ptr<CJavascriptVM> _host;
	weak_ptr<CPackage> _package;
	shared_ptr<CMetaData> _metaData;
	v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> _context;
};

}