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
	Thread(unsigned long (*function)(void), void *arg = 0);
	Thread(unsigned long (*function)(void*), void *arg = 0);
	~Thread();

	void Resume();
	void Pause();
	bool Wait(unsigned long duration = INFINITE);
private:
	bool started;
	void *thread;
};