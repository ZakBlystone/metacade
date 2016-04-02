/*
* <GLRenderThread.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "GLContextHost.h"
#include "GLPBuffer.h"

#pragma once

namespace Arcade
{
	//
	//A Thread that contains an OpenGL PBuffer to render to
	//
	class GLRenderThread
	{
	public:
		//Render callbacks from this thread
		class ICallbacks
		{
		public:
			virtual void Init() = 0;
			virtual void Draw() = 0;
			virtual void Shutdown() = 0;
		};

		//Creates a thread with a PBuffer of these dimensions
		GLRenderThread(int Width, int Height, ICallbacks *RenderCalls);
		
		//Destroys this thread
		~GLRenderThread();

		//Initialize this thread and attach its PBuffer to the HostContext
		bool Init(GLContextHost *HostContext);

		//Shutdown this thread and cleanup its resources
		void Shutdown();

		//Tell the thread to render a single frame
		void AdvanceFrame();

		//Check if the last requested frame is ready
		bool FrameIsPresentable();

		//Get PBuffer for this thread
		GLPBuffer *GetBuffer();

	private:
		friend unsigned long PBufferThread(void *renderInterface);

		GLPBuffer *Buffer;
		void *InterfaceHandle;
		bool Initialized;
		ICallbacks *RenderCalls;
	};
}