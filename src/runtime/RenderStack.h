/*
* <RenderStack.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include <queue>

#pragma once

#define RENDERCOMMAND_FIXED_SIZE 64

#undef REGISTER_RENDER_COMMAND
#define REGISTER_RENDER_COMMAND(StructType, Cmd) RC_##Cmd,

namespace Arcade
{

	enum RenderCommandFunc
	{
		RC_NOTHING,
#include "RenderStackCommands.h"
	};

	class RenderCommand
	{
	public:
		RenderCommand() : CommandFunc(RC_NOTHING) {}
		RenderCommandFunc CommandFunc;
		char Data[RENDERCOMMAND_FIXED_SIZE];
	};

	template<typename T, RenderCommandFunc CMD>
	class DefRenderCommand : public RenderCommand
	{

	public:
		typedef T StructType;

		DefRenderCommand()
		{
			static_assert(sizeof(T) < RENDERCOMMAND_FIXED_SIZE, "RenderCommand data bigger than RENDERCOMMAND_FIXED_SIZE");
			CommandFunc = CMD;
		}

		T &operator *()
		{
			return *(reinterpret_cast<T*>(Data));
		}

		T *Get() const
		{
			return (T*)Data;
		}

		static DefRenderCommand<T, CMD> *Initialize(RenderCommand *Base)
		{
			Base->CommandFunc = CMD;
			return static_cast<DefRenderCommand<T, CMD>*>(Base);
		}

		bool CheckFunc() { return CommandFunc == CMD; }
		RenderCommandFunc GetType() { return CommandFunc; }
		void Execute() const;
	};

	struct VoidData {};
	struct ColorData
	{
		float R, G, B, A;
	};

	struct TransformMatrix
	{
		float Mtx[9];
	};

	struct ImageDrawData
	{
		unsigned int Handle;
		float X, Y, W, H;
	};

	struct SpriteDrawData : public ImageDrawData
	{
		float R;
	};


#undef REGISTER_RENDER_COMMAND
#define REGISTER_RENDER_COMMAND(StructType, Cmd) typedef DefRenderCommand<StructType, RC_##Cmd> RenderCommand##Cmd;
#include "RenderStackCommands.h"
#undef REGISTER_RENDER_COMMAND

#define MAKE_RENDER_COMMAND(Stack,Cmd) Stack->Push<RenderCommand##Cmd>()->Get();


#define COMMAND_QUEUE_BASE_SIZE 8192

	class RenderStack
	{
	public:
		RenderStack();
		~RenderStack();

		template<typename T>
		T* Push()
		{
			static T Inst;
			CheckAndGrow();

			T* Value = static_cast<T*>(CommandQueuePtr);
			Value->CommandFunc = Inst.CommandFunc;

			++CommandQueueCount;
			++CommandQueuePtr;

			return Value;
		}
		bool Clear();
		bool Empty();
		void Execute();
		void CheckAndGrow();

	private:

		bool Locked;
		RenderCommand *CommandQueue, *CommandQueuePtr;
		int CommandQueueSize;
		int CommandQueueCount;
		//std::queue<RenderCommand> Commands;
	};

}