#include "hdr/gui/gui_label.h"
#include "hdr/gui/gui_textBox.h"
#include "hdr/gui/gui_button.h"
#include "hdr/gui/gui_checkBox.h"
#include "hdr/gui/gui_render.h"

//-----------------------------------------------------------------------------
//
// Draw a key based on passed in color and mode
void gui_drawObject ( int objectType, int whichObject, bool hasFocus )
//-----------------------------------------------------------------------------
{
	switch ( objectType )
	{
		case GUI_OBJECT_BUTTON:
			gui_drawButton( whichObject, hasFocus);
			break;

		case GUI_OBJECT_CHECKBOX:
			gui_drawCheckbox( whichObject, hasFocus );
			break;

		case GUI_OBJECT_TEXTBOX:
			gui_drawTextbox( whichObject, hasFocus );
			break;

		case GUI_OBJECT_LABEL:
			gui_drawLabel( whichObject, hasFocus );
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Main routine for drawing all the GUI screens
void gui_drawGUI ()
//-----------------------------------------------------------------------------
{
	//
	// Draw GUI object elements
	int indexCount = 0;

	if (!guiReady)
		return;

	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0x00);
	SDL_RenderClear(renderer);

	indexCount = 0;

// TODO - Drawing textboxes for scrSideView somehow
	for ( auto it = guiScreens[currentGUIScreen].objectIDIndex.begin (); it != guiScreens[currentGUIScreen].objectIDIndex.end (); ++it )
	{
		if ( guiScreens[currentGUIScreen].selectedObject == indexCount )
			gui_drawObject ( guiScreens[currentGUIScreen].objectType[indexCount], guiScreens[currentGUIScreen].objectIDIndex[indexCount], true );
		else
			gui_drawObject ( guiScreens[currentGUIScreen].objectType[indexCount], guiScreens[currentGUIScreen].objectIDIndex[indexCount], false );
		indexCount++;
	}
}
