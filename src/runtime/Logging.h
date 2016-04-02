/*
* <Logging.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#pragma once

namespace Arcade
{
	//
	//Types of log messages
	//
	enum LogMessageType
	{
		LOG_MESSAGE,
		LOG_WARN,
		LOG_ERROR,
	};

	//
	//Interface with the host application for logging
	//
	class IArcadeLogCallback
	{
	public:
		virtual void LogMessage(const char *Text, LogMessageType &Type) = 0;
	};

}