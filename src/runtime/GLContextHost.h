/*
* <GLContextHost.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#pragma once

namespace Arcade
{

	//
	//GLContextHost is an invisible OpenGL context which is used for creating PBuffers and uploading assets to video RAM
	//
	class GLContextHost
	{
	public:
		GLContextHost();

		bool Init();					//Initializes the context
		void Shutdown();				//Destroys the context

		void *GetDeviceContext();		//Returns a pointer to the DeviceContext (HDC*)
		void *GetRenderContext();		//Returns a pointer to the RenderContext (HGLRC*)

		bool MakeCurrent();				//Makes the context current on the calling thread
		void MakeNotCurrent();			//Clears the active GL context from the calling thread

	private:
		void RegisterWindowClass();		//Registers the internal window class
		void UnregisterWindowClass();	//Unregisters the internal window class

		//Pointers to internal types
		void *RenderContext;
		void *DeviceContext;
		void *Window;
	};

}