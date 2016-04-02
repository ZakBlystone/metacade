/*
* <LuaVM.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Runtime.h"

#pragma once

namespace Arcade {

	class CodeAsset;
	class RUNTIME_API LuaVM
	{
	public:

		bool Init();
		void Close();
		bool InstallCode(const char *Name, const char *Buffer, unsigned int Size);
		bool InstallCode(const char *Name, const char *Buffer);
		bool InstallCode(CodeAsset *Code);
		void InstallRuntime(Game *GameInstance);
		void InstallBuild(GamePackage *BuildTarget);
		void InstallMath();
		bool SetFunction(const char *Function);
		bool Call(int ReturnArgs = 0);
		void Push(int Value);
		void Push(float Value);
		void Push(const char *Value);
		bool GetGlobal(const char *Name);
		bool Get(int &Value);
		bool Get(float &Value);
		bool Get(char *Value, unsigned int BufferSize);
		unsigned int GetMemUsage();

	private:
		void *State;
		int CallArgs;
		unsigned int MemUsage;
	};

}