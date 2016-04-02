/*
* <GLPBuffer.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "GLContextHost.h"

#pragma once
namespace Arcade
{

	//
	//An OpenGL PBuffer context, RenderThreads render to these
	//
	class GLPBuffer
	{
	public:
		GLPBuffer(int Width, int Height);			//Create a context with this Width and Height

		bool Init(GLContextHost *HostContext);		//Initialize and share displaylists with the HostContext
		void Shutdown();							//Destroy the context
		void MakeCurrent();							//Make context current on thread
		void RenderToBitmap();						//Render the contents of the PBuffer to a bitmap

		void *GetBitmap();							//Get a pointer to the bitmap for this PBuffer
		int GetWidth();								//Get width of bitmap
		int GetHeight();							//Get height of bitmap

	private:

		//Pointers to internal types
		void *BufferHandle;
		void *BufferContext;
		void *BufferDC;
		void *Bitmap;

		//Bitmap info
		int BufferWidth;
		int BufferHeight;
		int BitmapSize;

		//PBO info
		int PBOIndex;
		bool UsePBO;
		unsigned int PBO[2];
	};
}