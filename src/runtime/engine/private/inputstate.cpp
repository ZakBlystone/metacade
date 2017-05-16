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
	setFocusElement(EFocusElement::FOCUS_KEYBOARD, focused);
}

void CInputState::setMouseIsFocused(bool focused)
{
	setFocusElement(EFocusElement::FOCUS_MOUSE, focused);
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

void CInputState::setKey(uint8 keycode, bool pressed)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_KEYBOARD;

	if ( pressed )
	{
		_keyboard[keycode] = 1;
	}
	else
	{
		_keyboard[keycode] = 0;
	}
}

bool CInputState::getKeyboardIsFocused() const
{
	return getFocusElement(EFocusElement::FOCUS_KEYBOARD);
}

bool CInputState::getMouseIsFocused() const
{
	return getFocusElement(EFocusElement::FOCUS_MOUSE);
}

void CInputState::getMousePosition(float &x, float &y) const
{
	x = _mouseX;
	y = _mouseY;
}

bool CInputState::getMouseButtonIsDown(EMouseButton button) const
{
	return (_mouseButtons & (1 << button)) != 0;
}

bool CInputState::getKeyIsDown(uint8 keycode) const
{
	return _keyboard[keycode] != 0;
}


uint8 CInputState::getStateFlags() const
{
	return _stateFlags;
}

void CInputState::setStateFlags(uint8 state)
{
	_stateFlags = state;
}

void CInputState::applyEvent(const CInputEvent& eventData)
{
	switch ( eventData.getEventType() )
	{
	case EInputEventType::INPUTEVENT_NONE:
	break;
	case EInputEventType::INPUTEVENT_MOUSEPRESSED:
		setMouseButton( eventData.getMouseButton(), true );
	break;
	case EInputEventType::INPUTEVENT_MOUSERELEASED:
		setMouseButton( eventData.getMouseButton(), false );
	break;
	case EInputEventType::INPUTEVENT_MOUSEMOVED:
		setMousePosition( eventData.getMouseX(), eventData.getMouseY() );
	break;
	case EInputEventType::INPUTEVENT_KEYPRESSED:
		setKey( eventData.getKeycode(), true );
	break;
	case EInputEventType::INPUTEVENT_KEYRELEASED:
		setKey( eventData.getKeycode(), false );
	break;
	case EInputEventType::INPUTEVENT_FOCUSGAINED:
		setFocusElement( eventData.getFocusElement(), true );
	break;
	case EInputEventType::INPUTEVENT_FOCUSLOST:
		setFocusElement( eventData.getFocusElement(), false );
	break;
	default:
	break;
	}
}

void CInputState::setFocusElement(EFocusElement focusElement, bool focused)
{
	_stateFlags |= EInputStateFlags::INPUTSTATE_FOCUS;

	if ( focused )
	{
		_focus |= (1 << focusElement);
	}
	else
	{
		_focus &= ~(1 << focusElement);
	}
}

bool CInputState::getFocusElement(EFocusElement focusElement) const
{
	return (_focus & (1 << focusElement)) != 0;
}