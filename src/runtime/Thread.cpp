/*
* <Thread.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include <thread>
#include <chrono>
#include <mutex>
#include <future>

#include "Thread.h"
#include "Runtime.h"

using namespace Arcade;

struct ThreadInfo
{
	std::thread *thread;
	std::mutex mutex;
	std::condition_variable waitcond;
	void *argument;
	unsigned long (*function)(void*);
};

int Thread::num_threads = 0;

unsigned long threadEntry(void *thread)
{
	Thread *t = (Thread *) thread;
	Runtime::LogPrint(LOG_MESSAGE, "Thread Init: %x", thread);

	{
	std::lock_guard<std::mutex> locktc(t->info->mutex);
	++Thread::num_threads;
	}

	unsigned long ret = t->info->function(t->info->argument);

	{
	std::lock_guard<std::mutex> locktc(t->info->mutex);
	--Thread::num_threads;
	}

	Runtime::LogPrint(LOG_MESSAGE, "Thread Exit: %x", thread);

	t->info->waitcond.notify_all();
	return ret;
}

Thread::Thread(unsigned long (*function)(void*), void *arg)
{
	Runtime::LogPrint(LOG_MESSAGE, "Thread Construct: %x", this);

	info = new ThreadInfo();
	info->function = function;
	info->argument = arg;
	info->thread = new std::thread(threadEntry, this);

	started = false;
}

Thread::~Thread()
{
	Runtime::LogPrint(LOG_MESSAGE, "Thread Destruct: %x", this);
	if ( info->thread->joinable() ) info->thread->join();

	Runtime::LogPrint(LOG_MESSAGE, "Thread **Destroy**: %x", this);

	delete info;
}

bool Thread::Wait(unsigned long duration)
{
	Runtime::LogPrint(LOG_MESSAGE, "Thread Wait: %x [%lld]", this, duration);

	std::thread *t = info->thread;
	if ( duration == INFINITE )
	{
		if ( info->thread->joinable() ) info->thread->join();
		return true;
	}

	std::mutex mutex;
	std::unique_lock<std::mutex> tlock(mutex);
	info->waitcond.wait_for(tlock, std::chrono::milliseconds(duration));

	Runtime::LogPrint(LOG_MESSAGE, "Thread Done Wait: %x [%lld]", this, duration);

	return true;
}

int Thread::GetNumThreads()
{
	return num_threads;
}
