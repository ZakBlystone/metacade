/*
* <ThreadEvent.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include <string>
#include <thread>
#include <future>

#include "ThreadEvent.h"
#include "Runtime.h"

using namespace Arcade;

static int eventIndex = 0;

struct EventInfo
{
	bool signaled;
	std::mutex mutex;
	std::condition_variable eventflag;
	EventInfo() : signaled(false) {}
};

ThreadEvent::ThreadEvent()
{
	info = new EventInfo;
}

ThreadEvent::~ThreadEvent()
{
	if ( info ) delete info;
}

bool ThreadEvent::IsValid()
{
	return true;
}

void ThreadEvent::Set()
{
	info->eventflag.notify_all();

	{
		std::lock_guard<std::mutex> signallock(info->mutex);
		info->signaled = true;
	}
}

void ThreadEvent::Reset()
{
	std::lock_guard<std::mutex> signallock(info->mutex);
	info->signaled = false;
}

bool ThreadEvent::Wait(unsigned long duration /*= INFINITE*/)
{
	{
		std::lock_guard<std::mutex> signallock(info->mutex);
		if ( info->signaled ) return true;
	}

	std::unique_lock<std::mutex> tlock(info->mutex);
	return info->eventflag.wait_for(tlock, std::chrono::milliseconds(duration)) == std::cv_status::no_timeout;
}

