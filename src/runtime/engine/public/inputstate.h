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

enum EFocusElement
{
	FOCUS_KEYBOARD,
	FOCUS_MOUSE,
	FOCUS_MAX,
};

enum EInputStateFlags
{
	INPUTSTATE_NONE,
	INPUTSTATE_MOUSEBUTTONS = 1 << 0,
	INPUTSTATE_MOUSEPOSITION = 1 << 1,
	INPUTSTATE_KEYBOARD = 1 << 2,
	INPUTSTATE_FOCUS = 1 << 3,
	INPUTSTATE_ALL = (1 << 4) - 1,
};

class METACADE_API CInputState
{
public:
	CInputState();

	void clear();
	void setKeyboardIsFocused(bool focused);
	void setMouseIsFocused(bool focused);
	void setMousePosition(float x, float y);
	void setMouseButton(EMouseButton button, bool pressed);
	void setKey(uint8 keycode, bool pressed);

	bool getKeyboardIsFocused() const;
	bool getMouseIsFocused() const;
	void getMousePosition(float &x, float &y) const;
	bool getMouseButtonIsDown(EMouseButton button) const;
	bool getKeyIsDown(uint8 keycode) const;
	uint8 getMouseButtonBits() const
	{
		return _mouseButtons;
	}

	void applyEvent(const class CInputEvent& eventData);
	void merge(const CInputState& other);

	template<typename Predicate>
	void generateEvents(const CInputState& previous, Predicate func) const
	{
		//mouse buttons
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_MOUSEBUTTONS )
		{
			for ( int32 i=0; i<EMouseButton::MOUSE_BUTTON_MAX; ++i )
			{
				bool oldState = (previous._mouseButtons & (1 << i)) != 0;
				bool newState = (_mouseButtons & (1 << i)) != 0;
				if ( oldState != newState )
				{
					func( CInputEvent::generateMouseButtonEvent( (EMouseButton) i, newState ) );
				}
			}
		}

		//mouse position
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_MOUSEPOSITION )
		{
			if ( previous._mouseX != _mouseX || previous._mouseY != _mouseY )
			{
				func( CInputEvent::generateMouseMovementEvent( previous._mouseX, previous._mouseY, _mouseX, _mouseY ) );
			}
		}

		//keys
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_KEYBOARD )
		{
			for ( int32 i=0; i<0xFF; ++i )
			{
				if ( previous._keyboard[i] != _keyboard[i] )
				{
					func( CInputEvent::generateKeyEvent( i, _keyboard[i] == 1 ) );
				}
			}
		}

		//focus
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_FOCUS )
		{
			for ( int32 i=0; i<EFocusElement::FOCUS_MAX; ++i )
			{
				bool oldState = (previous._focus & (1 << i)) != 0;
				bool newState = (_focus & (1 << i)) != 0;
				if ( oldState != newState )
				{
					func( CInputEvent::generateFocusEvent( (EFocusElement) i, newState ) );
				}
			}
		}
	}

	uint8 getStateFlags() const;
	void setStateFlags(uint8 state);

private:
	void setFocusElement(EFocusElement focusElement, bool focused);
	bool getFocusElement(EFocusElement focusElement) const;

	uint8 _keyboard[0xFF]; //state of all keys
	uint8 _stateFlags;
	uint8 _mouseButtons;
	uint8 _focus;
	float _mouseX;
	float _mouseY;
};

}