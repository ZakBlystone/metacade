/*
* <GLContextHost.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "GLContextHost.h"
#include "Runtime.h"
#include <Windows.h>
#include "gl/glew.h"
#include "gl/wglew.h"
#include "gl/glew.c"

//Name of window class for invisible window
#define OPENGL_HOST_CLASS_NAME "OpenGLHostContext"
//#define OPENGL_CONTEXT_40

using namespace Arcade;

//Flag so that there is only 1 window class
static bool WindowClassRegistered = false;

//Nullify window pointer
GLContextHost::GLContextHost() :
	Window(nullptr)
{

}

//Initializes the context
bool GLContextHost::Init()
{
	//Registers the window class
	RegisterWindowClass();

	//Create the invisible window
	HWND hWndFake = CreateWindow(OPENGL_HOST_CLASS_NAME, "FAKE", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, GetModuleHandle(0), NULL);

	//Error if the window wasn't created for some reason
	if (!hWndFake)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to create host context window");
		return false;
	}

	//Get window device context
	HDC hDC = GetDC(hWndFake);

	//Error if the window device context isn't valid
	if (!hDC)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to get DeviceContext for host context window");
		return false;
	}

	//Create a pixel format descriptor for OpenGL
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	//Choose a pixel format, error if failed
	int iPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (iPixelFormat == 0)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to find a suitable pixelformat for host context window");
		return false;
	}

	//Set the pixel format on the device context, error if failed
	if (!SetPixelFormat(hDC, iPixelFormat, &pfd))
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to set pixelformat for host context window");
		return false;
	}

	//Create the OpenGL context, error if failed
	HGLRC hRC = wglCreateContext(hDC);
	if (!hRC)
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to create a render context for host context window");
		return false;
	}

	//Make context current (only for a little bit), error if failed
	if (!wglMakeCurrent(hDC, hRC))
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to make host context window current");
		return false;
	}

	//Init pointers to OpenGL functions, error if failed
	if (glewInit() != GLEW_OK)
	{
		Runtime::LogPrint(LOG_ERROR, "GLEW Init failed on host context");
		return false;
	}

#ifdef OPENGL_CONTEXT_40
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);

	int iattribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		0, 0
	};

	hRC = wglCreateContextAttribsARB(hDC, NULL, iattribs);
#endif

	Runtime::LogPrint(LOG_MESSAGE, "Host GL context ready");

	//Clear context from main thread
	MakeNotCurrent();

	//Store pointers
	Window = hWndFake;
	RenderContext = hRC;
	DeviceContext = hDC;

	return true;
}

//Destroys the context
void GLContextHost::Shutdown()
{
	//Destroy the render context
	if (RenderContext) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext((HGLRC)RenderContext);

		RenderContext = nullptr;
	}

	//Destroy the invisible window
	if (Window) DestroyWindow((HWND)Window);
	UnregisterWindowClass();
}

//Returns a pointer to the DeviceContext (HDC*)
void * GLContextHost::GetDeviceContext()
{
	return DeviceContext;
}

//Returns a pointer to the RenderContext (HGLRC*)
void * GLContextHost::GetRenderContext()
{
	return RenderContext;
}

//Invisible window callback, don't paint anything
static LRESULT CALLBACK MsgHandlerSimpleOpenGLClass(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (uiMsg)
	{
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}
	return 0;
}

//Registers the internal window class
void GLContextHost::RegisterWindowClass()
{
	//Do not create a new window class if one already exists
	if (WindowClassRegistered) return;

	//Create a window class
	WNDCLASSEX wc;
	HINSTANCE hInstance = GetModuleHandle(0);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)MsgHandlerSimpleOpenGLClass;
	wc.cbClsExtra = 0; wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENUBAR + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = OPENGL_HOST_CLASS_NAME;

	RegisterClassEx(&wc);

	WindowClassRegistered = true;
}

//Unregisters the internal window class
void GLContextHost::UnregisterWindowClass()
{
	if (!WindowClassRegistered) return;
	UnregisterClass(OPENGL_HOST_CLASS_NAME, GetModuleHandle(0));
	WindowClassRegistered = false;
}

//Makes the context current on the calling thread
bool GLContextHost::MakeCurrent()
{
	if (!wglMakeCurrent((HDC)DeviceContext, (HGLRC)RenderContext))
	{
		Runtime::LogPrint(LOG_ERROR, "Unable to make host context window current");
		return false;
	}
	return true;
}

//Clears the active GL context from the calling thread
void Arcade::GLContextHost::MakeNotCurrent()
{
	wglMakeCurrent(NULL, NULL);
}
