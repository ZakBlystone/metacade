/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
inputstate.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

enum EMouseButton
{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_X1,
	MOUSE_BUTTON_X2,
	MOUSE_BUTTON_MAX,
};

enum EInputStateFlags
{
	INPUTSTATE_NONE,
	INPUTSTATE_MOUSEBUTTONS = 1 << 0,
	INPUTSTATE_MOUSEMOVED = 1 << 1,
	INPUTSTATE_KEYBOARD = 1 << 2,
	INPUTSTATE_MOUSEFOCUS = 1 << 3,
	INPUTSTATE_KEYBOARDFOCUS = 1 << 4,
};

class CInputState
{
public:
	CInputState();

	void clear();
	void setKeyboardIsFocused(bool focused);
	void setMouseIsFocused(bool focused);
	void setMousePosition(float x, float y);
	void setMouseButton(EMouseButton button, bool pressed);
	void setKey(uint8 keyboardScancode, bool pressed);

	CInputEvent getDiff(const CInputState& previous) const;

	uint8 getStateFlags() const;

private:
	uint8 _keyboard[0xFF]; //state of all keys
	uint8 _stateFlags;
	uint8 _mouseButtons;
	bool _keyboardFocus;
	bool _mouseFocus;
	float _mouseX;
	float _mouseY;
};

}