#include "hdr/gui/gui_textBox.h"
#include "hdr/gui/gui_main.h"

//--------------------------------------------------------------------------
//
// Draw a textbox onto the SDL renderer targeted surface
void gui_drawTextbox( int objectIndex, bool hasFocus )
//--------------------------------------------------------------------------
{
	Sint16      width, height;
	int         fontLineWidth, fontLineHeight;
	Sint16      startX, startY, gapSize, lineWidth;
	glm::vec2   labelPosition;
	glm::vec2   contentsPosition;
	SDL_Color   buttonColor;
	SDL_Color   cornerColor;
	SDL_Color   labelColor;

	if (guiTextBoxes.empty())
		return;

	if (!guiTextBoxes[objectIndex].attributes.positionCalled)
		return; // don't draw - has no position information

	startX = static_cast<Sint16>(guiTextBoxes[objectIndex].attributes.boundingBox.x);
	startY = static_cast<Sint16>(guiTextBoxes[objectIndex].attributes.boundingBox.y);
	width =  static_cast<Sint16>(guiTextBoxes[objectIndex].attributes.boundingBox.w);
	height = static_cast<Sint16>(guiTextBoxes[objectIndex].attributes.boundingBox.h);

	gapSize =   static_cast<Sint16>(guiTextBoxes[objectIndex].gapSize);
	lineWidth = static_cast<Sint16>(guiTextBoxes[objectIndex].lineWidth);

	if (hasFocus)
	{
		buttonColor = guiTextBoxes[objectIndex].attributes.hasFocusColor;
		labelColor = guiTextBoxes[objectIndex].attributes.labelHasFocusColor;
		cornerColor = guiTextBoxes[objectIndex].cornerFocusColor;
	}
	else
	{
		buttonColor = guiTextBoxes[objectIndex].attributes.noFocusColor;
		labelColor = guiTextBoxes[objectIndex].attributes.labelNoFocusColor;
		cornerColor = guiTextBoxes[objectIndex].cornerNoFocusColor;
	}

	roundedBoxRGBA (renderer, startX, startY, startX + width, startY + height, 8, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);

	roundedBoxRGBA (renderer, startX + lineWidth, startY + lineWidth, (startX + width) - lineWidth, (startY + height) - lineWidth, 8, 0, 0, 0, buttonColor.a);

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
	TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, guiTextBoxes[objectIndex].attributes.label.c_str(), &fontLineWidth, &fontLineHeight);

	labelPosition.x = ((float)startX - (fontLineWidth + gapSize));
	labelPosition.y = (float)startY + (((float)height - (float)fontLineHeight) / 2.0f);

	gui_renderText( guiFontName, labelPosition, glm::vec3{labelColor.r, labelColor.g, labelColor.b}, guiSurface, guiTextBoxes[objectIndex].attributes.label);

	contentsPosition.x = startX + gapSize;
	contentsPosition.y = (float)startY + (((float)height - (float)fontLineHeight) / 2.0f);

	if (!guiTextBoxes[objectIndex].contents.empty())
		gui_renderText ( guiFontName, contentsPosition, glm::vec3{labelColor.r, labelColor.g, labelColor.b}, guiSurface, guiTextBoxes[objectIndex].contents);
}
