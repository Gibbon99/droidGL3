#include "hdr/gui/gui_main.h"
#include "hdr/gui/gui_button.h"

SDL_TimerID         guiFocusTimerID;
SDL_Color           focusAnimateColor;
int                 focusAnimateIntervalValue;      // Set from script

//------------------------------------------------------------------------
//
// Callback for timer function - cursor flash
Uint32 gui_focusAnimateCallback ( Uint32 interval, void *param )
//------------------------------------------------------------------------
{
	static bool haveColorCopy = false;
	static bool fadeDown = true;

	if (guiButtons.empty())
		return interval;

	if (!haveColorCopy)
	{
		focusAnimateColor = guiButtons[0].cornerFocusColor;
		haveColorCopy = true;

		printf("Have copy of color [ %i %i %i ]\n", focusAnimateColor.r, focusAnimateColor.g, focusAnimateColor.b);
	}

	if (fadeDown)
	{
		if (focusAnimateColor.r > 0)
			focusAnimateColor.r -= 1;

		if (focusAnimateColor.g > 0)
			focusAnimateColor.g -= 1;


		if (focusAnimateColor.b > 0)
			focusAnimateColor.b -= 1;

		if ((focusAnimateColor.r == 0 ) && (focusAnimateColor.g == 0) && (focusAnimateColor.b == 0))
			fadeDown = false;
	}
	else
	{
		focusAnimateColor.r += 1;
		if (focusAnimateColor.r > guiButtons[0].cornerFocusColor.r)
			focusAnimateColor.r = guiButtons[0].cornerFocusColor.r;

		focusAnimateColor.g += 1;
		if (focusAnimateColor.g > guiButtons[0].cornerFocusColor.g)
			focusAnimateColor.g = guiButtons[0].cornerFocusColor.g;

		focusAnimateColor.b += 1;
		if (focusAnimateColor.b > guiButtons[0].cornerFocusColor.b)
			focusAnimateColor.b = guiButtons[0].cornerFocusColor.b;

		if ( focusAnimateColor.b == guiButtons[0].cornerFocusColor.b )
		{
			if ( focusAnimateColor.g == guiButtons[0].cornerFocusColor.g )
			{
				if ( focusAnimateColor.r == guiButtons[0].cornerFocusColor.r )
					fadeDown = true;
			}
		}
	}

	return interval;
}

//------------------------------------------------------------------------
//
// Use a timer to animate the colors for the active ( has focus ) button
void gui_timerFocusAnimation(int newState)
//------------------------------------------------------------------------
{
	switch ( newState )
	{
		case USER_EVENT_TIMER_OFF:
		{
			SDL_RemoveTimer (guiFocusTimerID);
			guiFocusTimerID = 0;
			break;
		}
		case USER_EVENT_TIMER_ON:
		{
			guiFocusTimerID = SDL_AddTimer ( focusAnimateIntervalValue, gui_focusAnimateCallback, nullptr );   // Time in milliseconds
			break;
		}
		default:
			break;
	}
}

//--------------------------------------------------------------------------
//
// Create a SDL_Surface and draw to it
void gui_drawButton( int objectIndex, bool hasFocus )
//--------------------------------------------------------------------------
{

	Sint16      width, height;
	int         fontLineWidth, fontLineHeight;
	Sint16      startX, startY, gapSize, lineWidth;
	glm::vec2   labelPosition;
	SDL_Color   buttonColor;
	SDL_Color   cornerColor;
	SDL_Color   labelColor;

	if (!guiButtons[objectIndex].attributes.positionCalled)
		return; // don't draw - has no position information

	startX = static_cast<Sint16>(guiButtons[objectIndex].attributes.boundingBox.x);
	startY = static_cast<Sint16>(guiButtons[objectIndex].attributes.boundingBox.y);
	width = static_cast<Sint16>(guiButtons[objectIndex].attributes.boundingBox.w);
	height = static_cast<Sint16>(guiButtons[objectIndex].attributes.boundingBox.h);

	gapSize = static_cast<Sint16>(guiButtons[objectIndex].gapSize);
	lineWidth = static_cast<Sint16>(guiButtons[objectIndex].lineWidth);

	if (hasFocus)
	{
		buttonColor = guiButtons[objectIndex].attributes.hasFocusColor;
		labelColor = guiButtons[objectIndex].attributes.labelHasFocusColor;
		cornerColor = guiButtons[objectIndex].cornerFocusColor;
	}
	else
	{
		buttonColor = guiButtons[objectIndex].attributes.noFocusColor;
		labelColor = guiButtons[objectIndex].attributes.labelNoFocusColor;
		cornerColor = guiButtons[objectIndex].cornerNoFocusColor;
	}

	roundedBoxRGBA (renderer, startX, startY, startX - width, startY + height, 8, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);

	roundedBoxRGBA (renderer, startX - lineWidth, startY + lineWidth, (startX - width) + lineWidth, (startY + height) - lineWidth, 8, 0, 0, 0, buttonColor.a);

	if (!hasFocus)
	{
		filledCircleRGBA ( renderer, (startX - width), static_cast<Sint16>(startY + (height / 2)), gapSize, cornerColor.r, cornerColor.g, cornerColor.b, cornerColor.a);
		filledCircleRGBA ( renderer, (startX - width), static_cast<Sint16>(startY + (height / 2)), static_cast<Sint16>(gapSize - 3), 0, 0, 0, cornerColor.a );
	}
	else
	{
		filledCircleRGBA ( renderer, (startX - width), static_cast<Sint16>(startY + (height / 2)), gapSize, cornerColor.r, cornerColor.g, cornerColor.b, cornerColor.a);
		filledCircleRGBA ( renderer, (startX - width), static_cast<Sint16>(startY + (height / 2)), static_cast<Sint16>(gapSize - 3), focusAnimateColor.r, focusAnimateColor.g, focusAnimateColor.b, cornerColor.a );
	}

	//
	// Draw the text label for the button
	TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, guiButtons[objectIndex].attributes.label.c_str(), &fontLineWidth, &fontLineHeight);
	switch (guiButtons[objectIndex].attributes.labelPos)
	{
		case GUI_LABEL_CENTER:
		{
			labelPosition.x = ((float)startX - (float)width) + (((float)width - (float)fontLineWidth) / 2.0f);
			labelPosition.y = (float)startY + (((float)height - (float)fontLineHeight) / 2.0f);;
			break;
		}
		case GUI_LABEL_LEFT:
		{
			labelPosition.x = startX;
			labelPosition.y = startY + ((height - (float) fontLineHeight) / 2);
			break;
		}
		case GUI_LABEL_RIGHT:
		{
			labelPosition.x = width - fontLineWidth;
			labelPosition.y = startY + ((height - (float) fontLineHeight) / 2);
			break;
		}

		default:
			break;
	}

	gui_renderText( guiFontName, labelPosition, glm::vec3{labelColor.r, labelColor.g, labelColor.b}, guiSurface, guiButtons[objectIndex].attributes.label);
}
