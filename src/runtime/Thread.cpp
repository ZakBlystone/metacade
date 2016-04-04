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
	std::promise<bool> done;
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
	t->info->done.set_value_at_thread_exit(true);
	return ret;
}

Thread::Thread(unsigned long (*function)(void*), void *arg)
{
	Runtime::LogPrint(LOG_MESSAGE, "Thread Construct: %x", this);

	info = new ThreadInfo();
	info->function = function;
	info->argument = arg;
	info->thread = new std::thread(threadEntry, this);
	info->thread->detach();

	started = false;
}

Thread::~Thread()
{
	Runtime::LogPrint(LOG_MESSAGE, "Thread Destruct: %x", this);
	std::future<bool> done = info->done.get_future();
	done.wait();

	Runtime::LogPrint(LOG_MESSAGE, "Thread **Destroy**: %x", this);

	delete info;
}

bool Thread::Wait(unsigned long duration)
{
	Runtime::LogPrint(LOG_MESSAGE, "Thread Wait: %x [%lld]", this, duration);

	std::thread *t = info->thread;
	if ( duration == INFINITE )
	{
		Runtime::LogPrint(LOG_MESSAGE, "Thread Wait-Join: %x", this);
		std::unique_lock<std::mutex> tlock(info->mutex);
		info->waitcond.wait(tlock);
		return true;
	}

	
	auto t0 = std::chrono::steady_clock::now();
	auto t1 = t0 + std::chrono::milliseconds(duration);
	std::unique_lock<std::mutex> tlock(info->mutex);
	info->waitcond.wait_until(tlock, t1);

	Runtime::LogPrint(LOG_MESSAGE, "Thread Done Wait: %x [%lld]", this, duration);

	return true;
}

int Thread::GetNumThreads()
{
	return num_threads;
}
