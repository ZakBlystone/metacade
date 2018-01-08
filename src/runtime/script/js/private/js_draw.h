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
js_draw.h: javascript drawing interface
===============================================================================
*/

#pragma once

#define NUM_MATRIX_STACK 16
#define NUM_CLIP_STACK 32

namespace Arcade
{
	struct CDrawInterface : public CRenderState
	{
		CElementRenderer* _renderer;
		CColor _currentColor;
		int32 _layer;
		CMatrix3 _xformStack[NUM_MATRIX_STACK];
		CClipShape _clipStack[NUM_CLIP_STACK];
		uint32 _xformStackPos;
		uint32 _clipStackPos;

		CDrawInterface();

		struct CRectData
		{
			float _u0, _v0;
			float _u1, _v1;
			float _x, _y;
			float _w, _h;
			CRectData() 
				: _u0(0.f)
				, _v0(0.f)
				, _u1(1.f)
				, _v1(1.f)
			{}
		};

		struct CSpriteData : public CRectData
		{
			float _r;
			CSpriteData() 
				: CRectData()
				, _r(0.f)
			{}
		};

		void start(CElementRenderer* renderer);
		void end();

		void resetStacks();
		bool xformPush();
		bool xformPop();
		bool clipPush();
		bool clipPop();

		CMatrix3& xformTop();
		CClipShape& clipTop();

		void rect(const CRectData& data, const CAssetRef* asset);
		void sprite(const CSpriteData& data, const CAssetRef* asset);
		void quad(CRenderQuad& data, const CAssetRef* asset);
		bool valid() const;
		inline CImageAsset* resolveTexture(const CAssetRef* asset);
		inline uint32 resolveTextureID(const CAssetRef* asset);
	};

	extern v8::Local<v8::ObjectTemplate> getJSDrawWrapper( v8::Isolate* isolate );
}