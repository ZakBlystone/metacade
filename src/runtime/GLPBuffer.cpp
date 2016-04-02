/*
* <GLPBuffer.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "gl/glew.h"
#include "gl/wglew.h"
#include "Runtime.h"
#include "GLPBuffer.h"
#include <Windows.h>

using namespace Arcade;

//
//GLPBuffer constructor:
//	Create a context with this Width and Height
//	Initializes pointers to nullptr
//
GLPBuffer::GLPBuffer(int Width, int Height) :
	BufferContext(nullptr),
	BufferDC(nullptr),
	BufferHandle(nullptr),
	Bitmap(nullptr),
	BufferWidth(Width),
	BufferHeight(Height),
	UsePBO(false)
{

}

//Initialize and share displaylists with the HostContext
bool GLPBuffer::Init(GLContextHost *HostContext)
{
	Shutdown();

	Runtime::LogPrint(LOG_MESSAGE, "Initialize PBuffer...");

	if (!HostContext)
	{
		Runtime::LogPrint(LOG_ERROR, "HostContext is null");
		return false;
	}

	//Cast host device context to (HDC*)
	HDC dc = (HDC)HostContext->GetDeviceContext();

	if (!HostContext)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to get DeviceContext from HostContext");
		return false;
	}

	Runtime::LogPrint(LOG_MESSAGE, "Host DeviceContext OK.");

	//Attributes for the PBuffer
	float fattributes[]
	{
		0, 0
	};
	int iattributes[]
	{
		WGL_DRAW_TO_PBUFFER_ARB, true,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_RED_BITS_ARB, 8,
			WGL_GREEN_BITS_ARB, 8,
			WGL_BLUE_BITS_ARB, 8,
			WGL_ALPHA_BITS_ARB, 8,
			0, 0,
	};

	if (!wglChoosePixelFormatARB)
	{
		Runtime::LogPrint(LOG_ERROR, "'wglChoosePixelFormatARB' is null!");
		if (wglChoosePixelFormatEXT)
		{
			Runtime::LogPrint(LOG_ERROR, "'wglChoosePixelFormatEXT' is a valid fallback!");
		}
		else
		{
			Runtime::LogPrint(LOG_ERROR, "no fallback!");
		}
		return false;
	}

	//Choose a pixelformat for this PBuffer
	int pixelformats[128];
	unsigned int nFormats = 0;
	if (!wglChoosePixelFormatARB(dc, iattributes, fattributes, 128, pixelformats, &nFormats))
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to obtain a pixel format!");
		return false;
	}

	Runtime::LogPrint(LOG_MESSAGE, " -PixelFormat");

	//Create PBuffer using attributes
	int iattribs[] = { 
		WGL_PBUFFER_LARGEST_ARB, true, 
		0, 0 
	};
	HPBUFFERARB hbuffer = wglCreatePbufferARB(dc, pixelformats[0], BufferWidth, BufferHeight, iattribs);
	if (!hbuffer)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to obtain a PBuffer!");
		return false;
	}

	Runtime::LogPrint(LOG_MESSAGE, " -PBufferARB");

	//Get device context for this PBuffer
	HDC PDC = wglGetPbufferDCARB(hbuffer);
	if (!PDC)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to obtain PBuffer device context!");
		wglDestroyPbufferARB(hbuffer);
		return false;
	}

	Runtime::LogPrint(LOG_MESSAGE, " -DeviceContext");

	//Get buffer size to determine if it is valid
	wglQueryPbufferARB(hbuffer, WGL_PBUFFER_WIDTH_ARB, &BufferWidth);
	wglQueryPbufferARB(hbuffer, WGL_PBUFFER_HEIGHT_ARB, &BufferHeight);

	//Not gonna work if width or height are less than 0
	if (BufferWidth <= 0 || BufferHeight <= 0)
	{
		Runtime::LogPrint(LOG_ERROR, "PBuffer size is not valid!");
		wglReleasePbufferDCARB(hbuffer, PDC);
		wglDestroyPbufferARB(hbuffer);
		return false;
	}

	Runtime::LogPrint(LOG_MESSAGE, " -SizeValidated");

	//Create an OpenGL context for this PBuffer
	HGLRC context = wglCreateContext(PDC);
	if (!context)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to create PBuffer render context!");
		wglReleasePbufferDCARB(hbuffer, PDC);
		wglDestroyPbufferARB(hbuffer);
		return false;
	}

	Runtime::LogPrint(LOG_MESSAGE, " -OpenGL");

	//Turn off VSync
	wglSwapIntervalEXT(0);

	Runtime::LogPrint(LOG_MESSAGE, " -NoVSync");

	//Share displaylists with host context (for loading assets)
	wglShareLists((HGLRC) HostContext->GetRenderContext(), context);

	Runtime::LogPrint(LOG_MESSAGE, " -ShareList");

	//Set pointers
	BufferContext = context;
	BufferDC = PDC;
	BufferHandle = hbuffer;

	//Set bitmap info
	BitmapSize = BufferWidth * BufferHeight * 3;
	Bitmap = Runtime::Realloc(Bitmap, BitmapSize);
	PBOIndex = 0;

	Runtime::LogPrint(LOG_MESSAGE, "PBuffer Created: %ix%i", BufferWidth, BufferHeight);

	return true;
}

void GLPBuffer::Shutdown()
{
	if (BufferContext)
	{
		if (UsePBO)
		{
			glDeleteBuffersARB(2, PBO);
		}

		wglDeleteContext((HGLRC)BufferContext);
	}

	if (BufferHandle)
	{
		wglReleasePbufferDCARB((HPBUFFERARB)BufferHandle, (HDC)BufferDC);
		wglDestroyPbufferARB((HPBUFFERARB)BufferHandle);
		Runtime::LogPrint(LOG_MESSAGE, "PBuffer Destroyed");
	}

	BufferContext = nullptr;
	BufferDC = nullptr;
	BufferHandle = nullptr;

	if (Bitmap)
	{
		Runtime::Free(Bitmap);
		Bitmap = nullptr;
	}
}

void GLPBuffer::MakeCurrent()
{
	if (BufferContext && BufferDC)
	{
		//Make this context current
		wglMakeCurrent((HDC)BufferDC, (HGLRC)BufferContext);

		//Parameters for glReadPixels
		glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
		glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
		glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_PACK_SKIP_ROWS, 0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		//Determine if PBOs can be used for Read-Back
		UsePBO = glewIsExtensionSupported("GL_ARB_pixel_buffer_object");
		if (UsePBO)
		{
			Runtime::LogPrint(LOG_MESSAGE, "Using PBO Read-Back");

			//Create 2 PixelBufferObjects, we'll use double-buffering to avoid blocking the RenderThread when reading the bitmap
			glGenBuffersARB(2, PBO);
			glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, PBO[0]);
			glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, BitmapSize, 0, GL_STREAM_READ_ARB);
			glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, PBO[1]);
			glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, BitmapSize, 0, GL_STREAM_READ_ARB);

			glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);

			Runtime::LogPrint(LOG_MESSAGE, "PBO initialized.");
		}
	}
}

//Render the contents of the PBuffer to a bitmap
void GLPBuffer::RenderToBitmap()
{
	//Reading from the front buffer
	glReadBuffer(GL_FRONT);

	//Using the PBO double buffering technique puts the RenderThread a frame behind the game
	if (UsePBO)
	{
		PBOIndex = (PBOIndex + 1) % 2;
		int NextIndex = (PBOIndex + 1) % 2;

		//Enqueue an asynchronous ReadPixels command on one buffer
		glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, PBO[PBOIndex]);
		glReadPixels(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_UNSIGNED_BYTE, 0);

		//And map the other buffer into RAM so we can get the result of the last read
		glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, PBO[NextIndex]);
		void *Data = (void *) glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
		if (Data)
		{
			//Copy pixels from the PBO into the bitmap
			memcpy(Bitmap, Data, BitmapSize);

			//Unmap the buffer from memory
			glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
		}
		else
		{
			Runtime::LogPrint(LOG_WARN, "PBO not ready yet");
		}
		
		//Unbind any PBOs
		glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	}
	else
	{
		//If PBOs aren't supported, just copy the pixels normally.
		//This function will block until all rendering is complete.
		glReadPixels(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_UNSIGNED_BYTE, Bitmap);
	}

	//Draw to the back buffer
	glDrawBuffer(GL_BACK);
}

//Get a pointer to the bitmap for this PBuffer
void * GLPBuffer::GetBitmap()
{
	return Bitmap;
}

//Get width of bitmap
int GLPBuffer::GetWidth()
{
	return BufferWidth;
}

//Get height of bitmap
int GLPBuffer::GetHeight()
{
	return BufferHeight;
}