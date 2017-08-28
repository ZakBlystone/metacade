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
	virtual void postInputState(const class CInputState& input) override;

	virtual void init() override;
	virtual void think(float time) override;
	virtual void render(IRenderer* renderer, CVec2 viewportSize, uint32 targetID = 0) override;
	virtual void initializeRenderer(IRenderer* renderer) override;
	virtual void finishRenderer(IRenderer* renderer) override;
	virtual bool callFunction(const CFunctionCall& call) override;

	virtual void initializeTextures(class ITextureProvider* provider) override;
	virtual void finishTextures(class ITextureProvider* provider) override;

	virtual void initSoundMixer(const CMixerSettings& settings) override;
	virtual ISoundMixer* getSoundMixer() override;

	virtual void setHostCallbacks(IHostCallbacks* callbacks) override;

	bool callHostFunction(const CFunctionCall& call, CVariant& returnValue);
	bool initializeParameters();
	shared_ptr<CPackage> getPackage() const;

private:
	CGameInstance(weak_ptr<CGameClass> klass, shared_ptr<IVMInstance> vmInstance);

	friend class CGameClass;

	weak_ptr<CGameClass> _klass;
	shared_ptr<IVMInstance> _vmInstance;
	shared_ptr<CElementRenderer> _elementRenderer;
	shared_ptr<class CSoundMixer> _mixer;

	shared_ptr<IImage> _defaultWhiteImage;
	map<IRenderer*, vector<ITexture* >*> _loadedTextures;

	vector<ITexture*>* getTextureList(IRenderer* renderer, bool newOnly = false);
	vector<ITexture*> _mainLoadedTextures;

	CInputState _inputState;
	CVec2 _desiredResolution;
	bool _hasDesiredResolution;

	float _lastTime;

	IHostCallbacks *_callbacks;

	CMatrix3 calculateAspectMatrix(const CVec2& viewport, const CVec2& desired) const;
	CMatrix3 getAspectMatrixForViewport(const CVec2& viewport) const;
};

}