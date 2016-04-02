/*
* <ThreadEvent.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#pragma once

class ThreadEvent {
public:
	ThreadEvent();
	~ThreadEvent();

	void Set();
	void Reset();
	bool IsValid();
	bool Wait(unsigned long duration = INFINITE);

private:
	bool started;
	void *eventHandle;
};