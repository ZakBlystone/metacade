/*
* <ThreadEvent.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include <Windows.h>
#include "ThreadEvent.h"
#include <string>

static int eventIndex = 0;

ThreadEvent::ThreadEvent() :
	eventHandle(nullptr)
{
	std::string eventName("Event");
	eventName += (eventIndex++);

	eventHandle = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		eventName.c_str());
}

ThreadEvent::~ThreadEvent()
{
	if (eventHandle)
	{
		CloseHandle((HANDLE)eventHandle);
	}
}

bool ThreadEvent::IsValid()
{
	return eventHandle != nullptr;
}

void ThreadEvent::Set()
{
	if (eventHandle) SetEvent((HANDLE)eventHandle);
}

void ThreadEvent::Reset()
{
	if (eventHandle) ResetEvent((HANDLE)eventHandle);
}

bool ThreadEvent::Wait(unsigned long duration /*= INFINITE*/)
{
	if (eventHandle) return WaitForSingleObject((HANDLE)eventHandle, duration) != WAIT_TIMEOUT;
	return false;
}

