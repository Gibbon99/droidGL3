#include <hdr/game/gam_render.h>
#include "hdr/io/io_mouse.h"

vec2 mousePosition{0,0};

SDL_TimerID         getMouseTimer;

//-----------------------------------------------------------------------------
//
// Get the current mouse position - used in GUI for action
SDL_Point io_getMousePointLocation()
//-----------------------------------------------------------------------------
{
	SDL_Point   mousePoint;

	mousePoint.x = mousePosition.x;
	mousePoint.y = mousePosition.y;

	return mousePoint;
}

//-----------------------------------------------------------------------------
//
// Called by the timer to read the mouse position
Uint32 io_getMousePositionCallback(Uint32 interval, void *ptr)
//-----------------------------------------------------------------------------
{
	int mouseX, mouseY;

	SDL_GetMouseState(&mouseX, &mouseY);

	mousePosition.x = mouseX;
	mousePosition.y = mouseY;

	evt_sendEvent (USER_EVENT_GUI, USER_EVENT_MOUSE_MOTION, 0, 0, 0, mousePosition, glm::vec2(), "");

	return interval;
}

//-----------------------------------------------------------------------------
//
// Start the mouse timer
void io_mouseTimerState(bool runState)
//-----------------------------------------------------------------------------
{
	static Uint32 timerInterval;

	if ( runState ? (timerInterval = 50) : (timerInterval = 0))

	getMouseTimer = SDL_AddTimer ( timerInterval, io_getMousePositionCallback, nullptr );   // Time in milliseconds
}

//-----------------------------------------------------------------------------
//
// Handle a mouse event
void io_handleMouseEvent ( SDL_Event event )
//-----------------------------------------------------------------------------
{
	switch ( currentMode )
	{
		case MODE_CONSOLE:
			break;

		case MODE_GUI:
			switch (event.type)
			{
				case SDL_MOUSEBUTTONDOWN:       // Set TRUE for mouse source - data3
					evt_sendEvent (USER_EVENT_GUI, USER_EVENT_KEY_EVENT, MY_INPUT_ACTION_PRESS, MY_INPUT_ACTION, true, glm::vec2{}, glm::vec2{},"");
					break;

				case SDL_MOUSEBUTTONUP:
					evt_sendEvent (USER_EVENT_GUI, USER_EVENT_KEY_EVENT, MY_INPUT_ACTION_RELEASE, MY_INPUT_ACTION, true, glm::vec2{}, glm::vec2{},"");
					break;

				default:
					break;
			}

		case MODE_PAUSE:
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Draw the mouse cursor
void io_renderMouseCursor()
//-----------------------------------------------------------------------------
{
#define lineSize    6

	switch (currentMode)
	{
		case MODE_GAME:
//			gl_drawLine (vec3 (mousePosition.x - (lineSize * 0.5), mousePosition.y, mousePosition.z), vec3 (mousePosition.x + (lineSize * 0.5), mousePosition.y, mousePosition.z), "colorLine", vec4 (1, 1, 1, 1));
//			gl_drawLine (vec3 (mousePosition.x, mousePosition.y - (lineSize * 0.5), mousePosition.z), vec3 (mousePosition.x, mousePosition.y + (lineSize * 0.5), mousePosition.z), "colorLine", vec4 (1, 1, 1, 1));

			gl_drawLine (vec3(winWidth / 2, 0, 0), vec3(winWidth / 2, winHeight, 0), "colorLine", vec4 (1, 1, 1, 1));
			gl_drawLine (vec3(0, winHeight / 2, 0), vec3(winWidth, winHeight / 2, 0), "colorLine", vec4 (1,1,1,1));
			break;

		default:
			break;
	}
}
