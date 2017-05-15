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
inputstate.cpp:
===============================================================================
*/

#include "engine_private.h"

CInputState::CInputState()
	: _stateFlags(INPUTSTATE_NONE)
	, _mouseButtons(0)
	, _keyboardFocus(false)
	, _mouseFocus(false)
	, _mouseX(0.f)
	, _mouseY(0.f)
{
	memset(_keyboard, 0, sizeof(uint8) * 0xFF);
}

void CInputState::clear()
{
	memset(_keyboard, 0, sizeof(uint8) * 0xFF);
	_mouseButtons = 0;
	_keyboardFocus = false;
	_mouseFocus = false;
	_mouseX = 0.f;
	_mouseY = 0.f;
	_stateFlags = 0;
}

void CInputState::setKeyboardIsFocused(bool focused)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_KEYBOARDFOCUS;
	_keyboardFocus = focused;
}

void CInputState::setMouseIsFocused(bool focused)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_MOUSEFOCUS;
	_mouseFocus = focused;
}

void CInputState::setMousePosition(float x, float y)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_MOUSEMOVED;
	_mouseX = x;
	_mouseY = y;
}

void CInputState::setMouseButton(EMouseButton button, bool pressed)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_MOUSEBUTTONS;
	
	if ( pressed )
	{
		_mouseButtons |= (1 << button);
	}
	else
	{
		_mouseButtons &= ~(1 << button);
	}
}

void CInputState::setKey(uint8 keyboardScancode, bool pressed)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_KEYBOARD;

	if ( pressed )
	{
		_keyboard[keyboardScancode] = 1;
	}
	else
	{
		_keyboard[keyboardScancode] = 0;
	}
}

CInputEvent CInputState::getDiff(const CInputState& previous) const
{
	return CInputEvent();
}

uint8 CInputState::getStateFlags() const
{
	return _stateFlags;
}
