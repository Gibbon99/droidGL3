#include <hdr/gui/gui_main.h>
#include "hdr/gui/gui_checkBox.h"

//----------------------------------------------------------------------------
//
// Called from script to update the state of the checked status
void gui_updateCheckedStatus(const string whichCheckBox, bool newState)
//----------------------------------------------------------------------------
{
	int whichCheckBoxIndex = gui_findIndex(GUI_OBJECT_CHECKBOX, whichCheckBox);

	guiCheckBoxes[whichCheckBoxIndex].checked = newState;
}



//----------------------------------------------------------------------------
//
// Draw a Checkbox
// Start is top left of box - width and height is just size of box
// Text length is not part of the overall size
void gui_drawCheckbox (int objectIndex, bool hasFocus )
//----------------------------------------------------------------------------
{
	Sint16      width, height;
	int         fontLineWidth, fontLineHeight;
	Sint16      startX, startY, gapSize, lineWidth;
	glm::vec2   labelPosition;
	SDL_Color   buttonColor;
	SDL_Color   checkColor;
	SDL_Color   labelColor;

	if (guiCheckBoxes.empty())
		return;

	if (!guiCheckBoxes[objectIndex].attributes.positionCalled)
		return; // don't draw - has no position information

	startX = static_cast<Sint16>(guiCheckBoxes[objectIndex].attributes.boundingBox.x);
	startY = static_cast<Sint16>(guiCheckBoxes[objectIndex].attributes.boundingBox.y);
	width = static_cast<Sint16>(guiCheckBoxes[objectIndex].attributes.boundingBox.w);
	height = static_cast<Sint16>(guiCheckBoxes[objectIndex].attributes.boundingBox.h);

	gapSize = static_cast<Sint16>(guiCheckBoxes[objectIndex].gapSize);
	lineWidth = static_cast<Sint16>(guiCheckBoxes[objectIndex].lineWidth);

	if (hasFocus)
	{
		buttonColor = guiCheckBoxes[objectIndex].attributes.hasFocusColor;
		labelColor = guiCheckBoxes[objectIndex].attributes.labelHasFocusColor;
		checkColor = guiCheckBoxes[objectIndex].cornerFocusColor;
	}
	else
	{
		buttonColor = guiCheckBoxes[objectIndex].attributes.noFocusColor;
		labelColor = guiCheckBoxes[objectIndex].attributes.labelNoFocusColor;
		checkColor = guiCheckBoxes[objectIndex].cornerNoFocusColor;
	}

	roundedBoxRGBA (renderer, startX + width, startY, startX, startY + height, 8, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);

	roundedBoxRGBA (renderer, (startX + width) - lineWidth, startY + lineWidth, startX + lineWidth, ( startY + height) - lineWidth, 8, 0, 0, 0, buttonColor.a);

	if (guiCheckBoxes[objectIndex].checked)
	{
		if (hasFocus)
			filledCircleRGBA ( renderer, startX + (width / 2), startY + (height / 2), (width - gapSize) / 2, focusAnimateColor.r, focusAnimateColor.g, focusAnimateColor.b, focusAnimateColor.a);
		else
			filledCircleRGBA ( renderer, startX + (width / 2), startY + (height / 2), (width - gapSize) / 2, checkColor.r, checkColor.g, checkColor.b, checkColor.a);
	}





	TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, guiButtons[objectIndex].attributes.label.c_str(), &fontLineWidth, &fontLineHeight);

	labelPosition.x = startX + width + gapSize;
	labelPosition.y = startY + ((height - fontLineHeight) / 2);
	gui_renderText( guiFontName, labelPosition, glm::vec3{labelColor.r, labelColor.g, labelColor.b}, guiSurface, guiCheckBoxes[objectIndex].attributes.label);
}
