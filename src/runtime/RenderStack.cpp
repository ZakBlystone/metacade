/*
* <RenderStack.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Runtime.h"
#include "RenderStack.h"
#include "gl/glew.h"
#include "Asset.h"

using namespace Arcade;

static void SwitchExecuteRenderCommand(RenderCommand *Command)
{
	#undef REGISTER_RENDER_COMMAND
	#define REGISTER_RENDER_COMMAND(StructType, Cmd) case RC_##Cmd: static_cast<const RenderCommand##Cmd *>(Command)->Execute(); break;

	RenderCommandFunc Function = Command->CommandFunc;
	switch (Function)
	{
	#include "RenderStackCommands.h"
	default: Runtime::LogPrint(LOG_ERROR, "Unknown render command!");  break;
	}

	#undef REGISTER_RENDER_COMMAND
}

#define COMMAND_FUNC(Cmd) template<> void RenderCommand##Cmd::Execute() const

#define CHECK_HANDLE(H) H != HANDLE_UNLOADED ? H : 0

COMMAND_FUNC(Image)
{
	ImageDrawData *Data = Get();
	glBindTexture(GL_TEXTURE_2D, CHECK_HANDLE(Data->Handle));
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(Data->X, Data->Y);
	glTexCoord2f(1, 0); glVertex2f(Data->X + Data->W, Data->Y);
	glTexCoord2f(1, 1); glVertex2f(Data->X + Data->W, Data->Y + Data->H);
	glTexCoord2f(0, 1); glVertex2f(Data->X, Data->Y + Data->H);
	glEnd();

	//Runtime::LogPrint(LOG_MESSAGE, "%i %f %f %f %f", Data->Handle, Data->X, Data->Y, Data->W, Data->H);
}

inline void xformspritevertex2f(float x, float y, float w, float h, float s, float c)
{
	float x1 = x + c*w - s*h;
	float y1 = y + s*w + c*h;
	glVertex2f(x1,y1);
}

#define QUICK_XFORM_VERTEX2f(x, y, w, h) xformspritevertex2f(x,y,w,h,S,C);

COMMAND_FUNC(Sprite)
{
	SpriteDrawData *Data = Get();
	float W = Data->W / 2;
	float H = Data->H / 2;
	float S = sinf(Data->R);
	float C = cosf(Data->R);
	glBindTexture(GL_TEXTURE_2D, CHECK_HANDLE(Data->Handle));
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); QUICK_XFORM_VERTEX2f(Data->X, Data->Y, -W, -H);
	glTexCoord2f(1, 0); QUICK_XFORM_VERTEX2f(Data->X, Data->Y, +W, -H);
	glTexCoord2f(1, 1); QUICK_XFORM_VERTEX2f(Data->X, Data->Y, +W, +H);
	glTexCoord2f(0, 1); QUICK_XFORM_VERTEX2f(Data->X, Data->Y, -W, +H);
	glEnd();
}

COMMAND_FUNC(Clear)
{
	ColorData *Data = Get();
	glClearColor(Data->R, Data->G, Data->B, Data->A);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

COMMAND_FUNC(SetColor)
{
	ColorData *Data = Get();
	glColor4f(Data->R, Data->G, Data->B, Data->A);
}

COMMAND_FUNC(PushTransform) 
{
	glPushMatrix();
}

COMMAND_FUNC(PopTransform) 
{
	glPopMatrix();
}

COMMAND_FUNC(Identity) 
{
	glLoadIdentity();
}

COMMAND_FUNC(BlendAdd)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

COMMAND_FUNC(BlendNormal)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

COMMAND_FUNC(BlendMultiply)
{
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
}

COMMAND_FUNC(MatrixMult) 
{
	float *M = Get()->Mtx;
	float Mtx[16] = {
		M[0], M[1], M[2], 0,
		M[3], M[4], M[5], 0,
		M[6], M[7], M[8], 0,
		0, 0, 0, 1,
	};

	glLoadMatrixf(Mtx);
}

void RenderStack::Execute()
{
	Locked = true;
	CommandQueuePtr = CommandQueue;
	while (CommandQueueCount > 0)
	{
		SwitchExecuteRenderCommand(CommandQueuePtr);
		++CommandQueuePtr;
		--CommandQueueCount;
	}
	CommandQueuePtr = CommandQueue;
	Locked = false;
}

bool RenderStack::Empty()
{
	return CommandQueueCount == 0;
}

bool RenderStack::Clear()
{
	//if (Locked) { Runtime::LogPrint(LOG_WARN, "Renderstack is locked!"); return false; }
	CommandQueueCount = 0;
	CommandQueuePtr = CommandQueue;
	return true;
}

Arcade::RenderStack::RenderStack() : Locked(false)
{
	CommandQueue = (RenderCommand *) Runtime::Realloc(nullptr, COMMAND_QUEUE_BASE_SIZE * sizeof(RenderCommand));
	CommandQueuePtr = CommandQueue;
	CommandQueueSize = COMMAND_QUEUE_BASE_SIZE;
	CommandQueueCount = 0;
}

Arcade::RenderStack::~RenderStack()
{
	Runtime::Free(CommandQueue);
}

void Arcade::RenderStack::CheckAndGrow()
{
	if (CommandQueueCount + 1 >= CommandQueueSize)
	{
		//Save queue in a buffer
		RenderCommand *Swap = (RenderCommand *) Runtime::Realloc(nullptr, CommandQueueSize * sizeof(RenderCommand));
		memcpy(Swap, CommandQueue, CommandQueueSize * sizeof(RenderCommand));

		//Increase size of queue
		int OldSize = CommandQueueSize;
		CommandQueueSize *= 2;

		//Re-allocate
		CommandQueue = (RenderCommand *)Runtime::Realloc(CommandQueue, CommandQueueSize * sizeof(RenderCommand));

		//Put old stuff back in
		memcpy(CommandQueue, Swap, OldSize * sizeof(RenderCommand));

		//Restore pointer location
		CommandQueuePtr = &CommandQueue[CommandQueueCount];

		//Remove swap buffer
		Runtime::Free(Swap);
		Runtime::LogPrint(LOG_MESSAGE, "Grow render command queue %i -> %i", OldSize, CommandQueueSize);
	}
}
