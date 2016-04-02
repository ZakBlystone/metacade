/*
* <Game.h>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "Runtime.h"

#pragma once

namespace Arcade
{
	//
	//Enum of controller / keyboard button states
	//
	enum ControlEventState
	{
		CSTATE_UP,
		CSTATE_PRESSED,
		CSTATE_DOWN,
		CSTATE_RELEASED,
	};

	//
	//Game represents a runnable arcade instance with a supplied Package of Assets
	//
	class LuaVM;
	class GamePackage;
	class GLRenderThread;
	class RenderStack;
	class RUNTIME_API Game
	{
	public:
		//Get the size of the arcade frame buffer
		void GetRenderSize(int &Width, int &Height);

		//Get a pointer to the pixels in the arcade frame buffer [RGB][8 bits per channel]
		char *GetFrameBufferRGB24Bit();

		//Not implemented yet
		void GetAudioSamples16BitMono(unsigned short *Stream, int DesiredSamples);

		//Run an update tick on the arcade VM, advances game time by 'DeltaTime'
		void Update(float DeltaTime);

		//Render a frame of the game in it's render context and copy it to the frame buffer
		void AdvanceFrame();

		//Determines if the requested frame has finished rendering
		bool FrameIsPresentable();

		//Send a controller button event to the arcade VM
		void ControllerEvent(const char *InputName, bool IsDown);

		//Get the state of a controller button
		ControlEventState GetControllerEventState(const char *InputName);

		GamePackage *GetPackage();			//GamePackage for this arcade instance
		RenderStack *GetRenderStack();		//Render stack for this arcade instance
		LuaVM *GetLuaVM();					//Lua virtual machine for this arcade instance

		//Get the averaged color of the frame buffer
		void GetAverageFrameBufferColor(float Color[3]);

	private:
		//These methods are only exposed to the ArcadeInstance and the Runtime class
		friend class Runtime;

		//Private constructor and destructor, only the Runtime may create / destroy arcade instances
		Game(GLContextHost *ContextHost, GamePackage *Package);
		~Game();

		float GameTime;
		float PreviousDT;
		GLRenderThread *RenderThread;
		GamePackage *Package;
		RenderStack *Render;
		LuaVM *VM;

		//STL types are not exposed outside the API, so these are pointers to those
		void *RenderCallbacks;
		void *ControlStateBuffer;
	};

}