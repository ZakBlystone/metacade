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
gameinstance.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

class CGameInstance : public CRuntimeObject, public IGameInstance
{
public:
	virtual class IGameClass* getClass() override;

	virtual void postInputEvent(const class CInputEvent& input) override;
	virtual void think(float time) override;
	virtual void render(IRenderer* renderer, CVec2 viewportSize, uint32 targetID = 0) override;
	virtual void initializeRenderer(IRenderer* renderer) override;
	virtual void finishRenderer(IRenderer* renderer) override;
	virtual bool callFunction(CFunctionCall call) override;

private:
	CGameInstance(weak_ptr<CGameClass> klass, shared_ptr<IVMInstance> vmInstance);

	friend class CGameClass;

	weak_ptr<CGameClass> _klass;
	shared_ptr<IVMInstance> _vmInstance;
	shared_ptr<CElementRenderer> _elementRenderer;

	shared_ptr<IImage> _defaultWhiteImage;
	vector<ITexture* > _loadedTextures;

	float _lastTime;
};

}