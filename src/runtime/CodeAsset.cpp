/*
* <CodeAsset.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "CodeAsset.h"
#include <cstring>

using namespace Arcade;

bool CodeAsset::Load(IFileObject *Object)
{
	//Get size of code string (size of asset itself)
	CodeBufferSize = GetSize();

	//Allocate code buffer, add 1 to the size and set to 0 so the string is null-terminated
	CodeBuffer = (char *) Runtime::Realloc(CodeBuffer, CodeBufferSize + 1);
	CodeBuffer[CodeBufferSize] = 0;

	//Read the code buffer from the file in the package
	if (!Object->Read(CodeBuffer, CodeBufferSize)) return false;
	return true;
}

bool CodeAsset::Save(IFileObject *Object)
{
	//If there isn't a code buffer, the asset is empty, so nothing to write.
	if (!CodeBuffer) return true;

	//Write the code buffer to the package
	return Object->Write(CodeBuffer, CodeBufferSize);
}

bool CodeAsset::Validate()
{
	//Inspect the binary string for unsavory characters
	for (int i = 0; i < CodeBufferSize; ++i)
	{
		//Extended Ascii characters are invalid for Lua code here
		if ((unsigned int)(CodeBuffer[i]) > 127) return false;
	}
	return true;
}

void CodeAsset::Release()
{
	//If the code buffer exists, free it from memory
	if (CodeBuffer)
	{
		delete[] CodeBuffer;
	}
	CodeBuffer = nullptr;
}

const char *CodeAsset::GetCodeBuffer()
{
	return CodeBuffer;
}

void CodeAsset::SetCodeBuffer(char *Buffer, int Size)
{
	//Release previous code buffer if there is one.
	Release();

	//Allocate code buffer and set buffer size
	CodeBuffer = (char *) Runtime::Realloc(CodeBuffer, Size);
	CodeBufferSize = Size;

	//Copy provided string into code buffer
	std::memcpy(CodeBuffer, Buffer, Size);
}
