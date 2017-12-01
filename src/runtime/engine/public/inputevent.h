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
inputevent.h:
===============================================================================
*/

#pragma once

namespace Arcade
{

enum EInputEventType
{
	INPUTEVENT_NONE,
	INPUTEVENT_MOUSEPRESSED,
	INPUTEVENT_MOUSERELEASED,
	INPUTEVENT_MOUSEMOVED,
	INPUTEVENT_KEYPRESSED,
	INPUTEVENT_KEYRELEASED,
	INPUTEVENT_FOCUSGAINED,
	INPUTEVENT_FOCUSLOST,
};

class METACADE_API CInputEvent
{
public:
	static CInputEvent generateKeyEvent(uint8 keycode, bool wasPressed);
	static CInputEvent generateMouseButtonEvent(EMouseButton button, bool wasPressed);
	static CInputEvent generateMouseMovementEvent(float oldX, float oldY, float x, float y);
	static CInputEvent generateFocusEvent(EFocusElement element, bool hasFocus);

	EInputEventType getEventType() const;

	EFocusElement getFocusElement() const;
	EMouseButton getMouseButton() const;
	uint8 getKeycode() const;
	float getMouseX() const;
	float getMouseY() const;
	float getMouseDeltaX() const;
	float getMouseDeltaY() const;

	void applyTransform(const CMatrix3& matrix);
	bool canApplyTransform() const;

	CInputEvent getTransformedEvent(const CMatrix3& matrix) const;

private:
	CInputEvent();

	EInputEventType _eventType;
	EFocusElement _focusElement;
	EMouseButton _mouseButton;
	uint8 _keycode;
	float _mouseX;
	float _mouseY;
	float _mouseDeltaX;
	float _mouseDeltaY;
};

}