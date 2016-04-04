/*
* <Thread.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#pragma once

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

class Thread {
public:
	Thread(unsigned long (*function)(void*), void *arg = 0);
	~Thread();

	bool Wait(unsigned long duration = INFINITE);

	static int GetNumThreads();

private:
	bool started;

	struct ThreadInfo *info;

	friend unsigned long threadEntry(void *thread);
	static int num_threads;
};