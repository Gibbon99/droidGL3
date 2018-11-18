#include <hdr/game/gam_render.h>
#include "hdr/io/io_mouse.h"

vec3 mousePosition{0,0,0};

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

		case MODE_GAME:
			if ( event.type == SDL_MOUSEMOTION )
			{
				mousePosition.x = event.motion.x;
				mousePosition.y = winHeight - event.motion.y;
				mousePosition.z = 0.0f;
			}
			break;

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

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
glm::vec3 io_getMousePosition()
//-----------------------------------------------------------------------------------------------------
{
	return mousePosition;
}