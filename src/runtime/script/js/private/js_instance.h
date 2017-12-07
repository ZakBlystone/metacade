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
js_instance.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CJavascriptVMInstance : public IVMInstance
{
public:
	CJavascriptVMInstance(weak_ptr<CJavascriptVMClass> klass);

	virtual IVMClass* getClass() override;

	virtual void setGameInstance(IGameInstance* gameInstance) override;
	virtual void postInputEvent(const class CInputEvent& input) override;
	virtual void init() override;

	virtual void think(float seconds, float deltaSeconds) override;
	virtual void render(shared_ptr<class CElementRenderer> renderer) override;
	virtual void reset() override;

	virtual bool callFunction(const CFunctionCall& call) override;
private:
	weak_ptr<CJavascriptVMClass> _klass;
};

}