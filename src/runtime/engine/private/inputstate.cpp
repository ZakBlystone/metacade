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
	, _focus(0)
	, _mouseX(0.f)
	, _mouseY(0.f)
{
	memset(_keyboard, 0, sizeof(uint8) * 0xFF);
}

void CInputState::clear()
{
	memset(_keyboard, 0, sizeof(uint8) * 0xFF);
	_stateFlags = 0;
	_mouseButtons = 0;
	_focus = 0;
	_mouseX = 0.f;
	_mouseY = 0.f;
}

void CInputState::setKeyboardIsFocused(bool focused)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_FOCUS;

	if ( focused )
	{
		_focus |= (1 << EFocusElement::FOCUS_KEYBOARD);
	}
	else
	{
		_focus &= ~(1 << EFocusElement::FOCUS_KEYBOARD);
	}
}

void CInputState::setMouseIsFocused(bool focused)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_FOCUS;

	if ( focused )
	{
		_focus |= (1 << EFocusElement::FOCUS_MOUSE);
	}
	else
	{
		_focus &= ~(1 << EFocusElement::FOCUS_MOUSE);
	}
}

void CInputState::setMousePosition(float x, float y)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_MOUSEPOSITION;
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

uint8 CInputState::getStateFlags() const
{
	return _stateFlags;
}
