#include "hdr/gui/gui_label.h"
#include "hdr/gui/gui_main.h"

//--------------------------------------------------------------------------
//
// Draw a textbox onto the SDL renderer targeted surface
void gui_drawLabel( int objectIndex, bool hasFocus )
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

	if (guiLabels.empty())
		return;

	if (!guiLabels[objectIndex].positionCalled)
		return; // don't draw - has no position information

	startX = static_cast<Sint16>(guiLabels[objectIndex].boundingBox.x);
	startY = static_cast<Sint16>(guiLabels[objectIndex].boundingBox.y);

	labelColor = guiTextBoxes[objectIndex].attributes.labelHasFocusColor;

	//
	// Draw the text label for the button
	TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, guiLabels[objectIndex].label.c_str(), &fontLineWidth, &fontLineHeight);

	labelPosition.x = startX;
	labelPosition.y = startY;

	// TODO: Use a different font for labels - something bigger
	// Set font name from script - and then use to pass into object?
	gui_renderText( guiFontName, labelPosition, glm::vec3{labelColor.r, labelColor.g, labelColor.b}, guiSurface, guiLabels[objectIndex].label);
}
