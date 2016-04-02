/*
* <Thread.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include <Windows.h>
#include "Thread.h"

Thread::Thread(unsigned long (*function)(void), void *arg)
{
	thread = CreateThread(0,
		0,
		(LPTHREAD_START_ROUTINE) function,
		arg,
		CREATE_SUSPENDED,
		0);

	started = false;
}

Thread::Thread(unsigned long (*function)(void*), void *arg)
{
	thread = CreateThread(0,
		0,
		(LPTHREAD_START_ROUTINE) function,
		arg,
		CREATE_SUSPENDED,
		0);

	started = false;
}

Thread::~Thread()
{
	CloseHandle((HANDLE)thread);
}

void Thread::Resume()
{
	if(started) return;
	ResumeThread((HANDLE)thread);
	started = true;
}

void Thread::Pause()
{
	if(!started) return;
	SuspendThread((HANDLE)thread);
	started = false;
}

bool Thread::Wait(unsigned long duration)
{
	return WaitForSingleObject((HANDLE)thread, duration) != WAIT_TIMEOUT;
}