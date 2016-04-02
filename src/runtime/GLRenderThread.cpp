/*
* <GLRenderThread.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "GLRenderThread.h"
#include "Thread.h"
#include "ThreadEvent.h"
#include "Runtime.h"
#include "gl/glew.h"

using namespace Arcade;

//
//Interface between the main thread and the RenderThread
//
struct GLInterface
{
	//Events for triggering the thread
	ThreadEvent FrameAdvanceEvent, FrameFinishedEvent;

	Thread *RenderThread;	//Thread instance
	GLPBuffer *Buffer;		//PBuffer
	GLRenderThread *Host;	//RenderThread
	bool Running;			//Kill the thread by setting this to false and raising the FrameAdvanceEvent

	GLInterface() 
	{
		//Start as having finished a frame so we don't wait on nothing
		FrameFinishedEvent.Set();
	}

	void AdvanceRenderThread()
	{
		//Skip if last frame isn't ready yet.
		if ( !FrameFinishedEvent.Wait(0) )
		{
			Runtime::LogPrint(LOG_WARN, "RenderThread[%X] skipping frame.", (int)this);
			return;
		}

		//Reset the finished event
		FrameFinishedEvent.Reset();

		//Raise the FrameAdvanceEvent so that the thread will stop blocking and render
		FrameAdvanceEvent.Set();
	}
};

//Forward declaration for thread code
unsigned long PBufferThread(void *renderInterface);

//
//GLRenderThread constructor:
//	Create a PBuffer with this Width and Height
//	Optional Callbacks
//
GLRenderThread::GLRenderThread(int Width, int Height, ICallbacks *Callbacks = nullptr) :
	Initialized(false),
	RenderCalls(Callbacks),
	InterfaceHandle(nullptr)
{
	Runtime::LogPrint(LOG_MESSAGE, "Render thread constructor...");
	Buffer = new GLPBuffer(Width, Height);
	Runtime::LogPrint(LOG_MESSAGE, "PBuffer created.");
}

//
//GLRenderThread destructor, shuts down the thread and deletes the PBuffer
//
GLRenderThread::~GLRenderThread()
{
	Shutdown();
	delete Buffer;
}

//Initialize this thread and attach its PBuffer to the HostContext
bool GLRenderThread::Init(GLContextHost *HostContext)
{
	//If the thread is already initialized, do nothing
	if (Initialized) return false;

	//Initialize PBuffer with HostContext
	if (!Buffer->Init(HostContext))
	{
		return false;
	}

	//Create Thread and Interface
	GLInterface *GLI = new GLInterface;
	Thread *PTHREAD = new Thread(PBufferThread, GLI);

	Runtime::LogPrint(LOG_MESSAGE, "Startup render thread[%X]...", (int)GLI);

	//Keep a pointer to the interface
	InterfaceHandle = GLI;

	//Setup interface and start the thread
	GLI->Buffer = Buffer;
	GLI->RenderThread = PTHREAD;
	GLI->Running = true;
	GLI->RenderThread->Resume();
	GLI->Host = this;

	//Flag this RenderThread as initialized
	Initialized = true;
	return true;
}

//Shutdown this thread and cleanup its resources
void GLRenderThread::Shutdown()
{
	//If the thread isn't initialized, do nothing
	if (Initialized)
	{
		Initialized = false;
		GLInterface *GLI = (GLInterface *)InterfaceHandle;

		Runtime::LogPrint(LOG_MESSAGE, "Shutting down render thread[%X]...", (int)GLI);

		if (GLI)
		{
			//Turn off the 'Running' flag and spin the thread so it shuts down
			GLI->Running = false;
			GLI->FrameAdvanceEvent.Set();

			//Wait for a second, if the thread has stalled, there's a problem
			if (!GLI->RenderThread->Wait(1000))
			{
				Runtime::LogPrint(LOG_ERROR, "Thread lockup on shutdown[%X] Forcing...", (int)GLI);
			}
		}

		//Shutdown the PBuffer
		Buffer->Shutdown();

		Runtime::LogPrint(LOG_MESSAGE, "Finished thread shutdown[%X]", (int)GLI);

		//Delete interface
		delete GLI;
		GLI = nullptr;
	}
}

//Tell the thread to render a single frame
void GLRenderThread::AdvanceFrame()
{
	GLInterface *GLI = (GLInterface *)InterfaceHandle;

	if (!GLI) return;

	GLI->AdvanceRenderThread();
}

//Check if the last requested frame is ready
bool GLRenderThread::FrameIsPresentable()
{
	GLInterface *GLI = (GLInterface *)InterfaceHandle;

	if (!GLI) return false;

	return GLI->FrameFinishedEvent.Wait(0);
}

//Get PBuffer for this thread
GLPBuffer * GLRenderThread::GetBuffer()
{
	return Buffer;
}

//This code executes on the RenderThread
unsigned long Arcade::PBufferThread(void *renderInterface)
{
	//Get interface to RenderThread
	GLInterface *GLI = (GLInterface *)renderInterface;
	GLRenderThread::ICallbacks *CALL = GLI->Host->RenderCalls;

	//Make PBuffer current on this thread
	GLI->Buffer->MakeCurrent();

	//Call the 'Init' function in the RenderThreads callbacks
	if (CALL) CALL->Init();

	while (true)
	{
		//Wait for a command to render a frame
		GLI->FrameAdvanceEvent.Wait();

		//Render the PBuffer to a bitmap
		GLI->Buffer->RenderToBitmap();

		//IF the thread was killed break out of this loop
		if (!GLI->Running) break;

		//Call the 'Draw' function in the RenderThreads callbacks
		if (CALL) CALL->Draw();

		//Finished, reset the advance command so that it waits again
		GLI->FrameAdvanceEvent.Reset();

		//Tell the main thread that the frame is ready
		GLI->FrameFinishedEvent.Set();
	}

	//Call the 'Shutdown' function in the RenderThreads callbacks
	if (CALL) CALL->Shutdown();

	return 0;
}