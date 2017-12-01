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
inputevent.cpp:
===============================================================================
*/

#include "engine_private.h"

CInputEvent::CInputEvent()
	: _eventType(EInputEventType::INPUTEVENT_NONE)
	, _focusElement(EFocusElement::FOCUS_KEYBOARD)
	, _keycode(0)
	, _mouseButton(EMouseButton::MOUSE_BUTTON_LEFT)
	, _mouseX(0.f)
	, _mouseY(0.f)
	, _mouseDeltaX(0.f)
	, _mouseDeltaY(0.f)
{

}

CInputEvent CInputEvent::generateKeyEvent(uint8 keycode, bool wasPressed)
{
	CInputEvent outEvent;
	outEvent._keycode = keycode;
	outEvent._eventType = wasPressed 
		? EInputEventType::INPUTEVENT_KEYPRESSED 
		: EInputEventType::INPUTEVENT_KEYRELEASED;

	return outEvent;
}

CInputEvent CInputEvent::generateMouseButtonEvent(EMouseButton button, bool wasPressed)
{
	CInputEvent outEvent;
	outEvent._mouseButton = button;
	outEvent._eventType = wasPressed 
		? EInputEventType::INPUTEVENT_MOUSEPRESSED 
		: EInputEventType::INPUTEVENT_MOUSERELEASED;

	return outEvent;
}

CInputEvent CInputEvent::generateMouseMovementEvent(float oldX, float oldY, float x, float y)
{
	CInputEvent outEvent;
	outEvent._eventType = EInputEventType::INPUTEVENT_MOUSEMOVED;
	outEvent._mouseX = x;
	outEvent._mouseY = y;
	outEvent._mouseDeltaX = x - oldX;
	outEvent._mouseDeltaY = y - oldY;

	return outEvent;
}

CInputEvent CInputEvent::generateFocusEvent(EFocusElement element, bool hasFocus)
{
	CInputEvent outEvent;
	outEvent._focusElement = element;
	outEvent._eventType = hasFocus 
		? EInputEventType::INPUTEVENT_FOCUSGAINED 
		: EInputEventType::INPUTEVENT_FOCUSLOST;

	return outEvent;
}

EInputEventType CInputEvent::getEventType() const
{
	return _eventType;
}

EFocusElement CInputEvent::getFocusElement() const
{
	return _focusElement;
}

EMouseButton CInputEvent::getMouseButton() const
{
	return _mouseButton;
}

uint8 CInputEvent::getKeycode() const
{
	return _keycode;
}

float CInputEvent::getMouseX() const
{
	return _mouseX;
}

float CInputEvent::getMouseY() const
{
	return _mouseY;
}

float CInputEvent::getMouseDeltaX() const
{
	return _mouseDeltaX;
}

float CInputEvent::getMouseDeltaY() const
{
	return _mouseDeltaY;
}

void CInputEvent::applyTransform(const CMatrix3& matrix)
{
	if ( getEventType() != INPUTEVENT_MOUSEMOVED ) return;

	float newMouseX = _mouseX * matrix(0,0) + _mouseY * matrix(0,1) + matrix(0,2);
	float newMouseY = _mouseX * matrix(1,0) + _mouseY * matrix(1,1) + matrix(1,2);
	float newMouseDeltaX = _mouseDeltaX * matrix(0,0) + _mouseDeltaY * matrix(0,1);
	float newMouseDeltaY = _mouseDeltaX * matrix(1,0) + _mouseDeltaY * matrix(1,1);

	_mouseX = newMouseX;
	_mouseY = newMouseY;
	_mouseDeltaX = newMouseDeltaX;
	_mouseDeltaY = newMouseDeltaY;
}

bool CInputEvent::canApplyTransform() const
{
	return getEventType() == INPUTEVENT_MOUSEMOVED;
}

CInputEvent CInputEvent::getTransformedEvent(const CMatrix3& matrix) const
{
	CInputEvent newEvent = *this;
	newEvent.applyTransform(matrix);
	return newEvent;
}

