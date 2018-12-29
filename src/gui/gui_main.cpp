#include <utility>
#include "hdr/gui/gui_scrollBox.h"

#include "hdr/libGL/sdl2_gfx/SDL2_rotozoom.h"
#include "hdr/system/sys_timing.h"
#include "hdr/io/io_logfile.h"
#include "hdr/system/sys_audio.h"
#include "hdr/io/io_mouse.h"
#include "hdr/gui/gui_button.h"
#include "hdr/game/gam_hud.h"

#include "hdr/gui/gui_main.h"
#include "hdr/gui/gui_render.h"
#include "hdr/system/sys_sdfFont.h"

//#define DEBUG_GUI_SETUP 1

SDL_Renderer                            *renderer = nullptr;
SDL_Surface                             *guiSurface = nullptr;
vector<_font>                           ttfFonts;
unordered_map <string, unsigned int>    ttfFontIndex;
bool                                    guiReady = false;

int				                        currentGUIScreen;
int                                     currentObjectSelected;  // Pass this to script to act on

SDL_Color                               focusAnimateColor;

vector<_screenObject>	                guiScreens;
vector<_guiButton>                      guiButtons;
vector<_guiCheckBox>                    guiCheckBoxes;
vector<_guiTextBox>                     guiTextBoxes;
vector<_guiObject>                      guiLabels;

std::string                             guiFontName;

_screenObject                           tmpScreen;
_guiButton                              tmpGuiButton;
_guiCheckBox                            tmpGuiCheckBox;
_guiTextBox                             tmpGuiTextBox;
_guiObject                              tmpLabel;

//----------------------------------------------------------------
//
// Handle a GUI event - called by thread
int gam_processGuiEventQueue ( void *ptr )
//----------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay ( THREAD_DELAY_MS );

		if ( !guiEventQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex ( guiMutex ) == 0 )
			{
				tempEventData = guiEventQueue.front ();
				guiEventQueue.pop ();
				SDL_UnlockMutex ( guiMutex );
			}

			switch ( tempEventData.eventAction )
			{
				case USER_EVENT_MOUSE_MOTION:
					gui_handleMouseMotion(tempEventData.vec2_1);
					break;

				case USER_EVENT_KEY_EVENT:
					//
					// Call the function associated with this element
					gui_handleInputEvent(tempEventData.data1, tempEventData.data2, tempEventData.data3);
					break;

				case USER_EVENT_GAME_TIMER:
					switch ( tempEventData.data1 )
					{
						case USER_EVENT_GUI_MOUSE_TIMER:
							io_mouseTimerState ( tempEventData.data2 );
							break;

						case USER_EVENT_GUI_ANIMATE_TIMER:
							gui_timerFocusAnimation( tempEventData.data2 );
							break;

						default:
							break;
					}
					break;

				default:
					break;
			}
		}
	}
	printf ("GUI thread stopped.\n");
	return 0;
}


//-----------------------------------------------------------------------------
//
// Find the objectID on the current screen and make it selected
void gui_setObjectFocus(string objectID)
//-----------------------------------------------------------------------------
{
	int indexCount = 0;

	for (indexCount = 0; indexCount != (int)guiScreens[currentGUIScreen].objectIDIndex.size(); indexCount++)
	{
		switch (guiScreens[currentGUIScreen].objectType[indexCount])
		{
			case GUI_OBJECT_BUTTON:
				if (guiButtons[guiScreens[currentGUIScreen].objectIDIndex[indexCount]].attributes.objectID == objectID)
				{
					guiScreens[currentGUIScreen].selectedObject = indexCount;
					return;
				}
				break;

			case GUI_OBJECT_CHECKBOX:
				if (guiCheckBoxes[guiScreens[currentGUIScreen].objectIDIndex[indexCount]].attributes.objectID == objectID)
				{
					guiScreens[currentGUIScreen].selectedObject = indexCount;
					return;
				}
				break;

			case GUI_OBJECT_TEXTBOX:
				if (guiTextBoxes[guiScreens[currentGUIScreen].objectIDIndex[indexCount]].attributes.objectID == objectID)
				{
					guiScreens[currentGUIScreen].selectedObject = indexCount;
					return;
				}
				break;
		}
	}
	io_logToFile("GUI Error: ObjectID [ %s ] not found", objectID.c_str());
}


//-----------------------------------------------------------------------------
//
// Look through the relevant vector to locate the index of the objectID
//
// returns index value, or -1 if not found
int gui_findIndex(int guiObjectType, const string objectID)
//-----------------------------------------------------------------------------
{
	int indexCount = 0;

	indexCount = 0;

	switch ( guiObjectType )
	{
		case GUI_OBJECT_SCREEN:
			for ( const auto &iter : guiScreens )
			{
				if ( iter.screenID == objectID )
				{
					return indexCount;
				}
				indexCount++;
			}
			return -1;
			break;

		case GUI_OBJECT_BUTTON:
			for ( const auto &iter : guiButtons )
			{
				if (iter.attributes.objectID == objectID)
				{
					return indexCount;
				}
				indexCount++;
			}
			return -1;  // Didn't find a match
			break;

		case GUI_OBJECT_CHECKBOX:
			for ( const auto &iter : guiCheckBoxes )
			{
				if (iter.attributes.objectID == objectID)
				{
					return indexCount;
				}
				indexCount++;
			}
			return -1;  // Didn't find a match
			break;

		case GUI_OBJECT_TEXTBOX:
			for ( const auto &iter : guiTextBoxes )
			{
				if (iter.attributes.objectID == objectID)
				{
					return indexCount;
				}
				indexCount++;
			}
			return -1;  // Didn't find a match
			break;

		case GUI_OBJECT_LABEL:
			for ( const auto &iter : guiLabels )
			{
				if (iter.objectID == objectID)
				{
					return indexCount;
				}
				indexCount++;
			}
			return -1;  // Didn't find a match
			break;

		default:
			break;
	}
	return -1;      // Should never get here
}

//-----------------------------------------------------------------------------
//
// Add a gui object to it's relevant vector array
void gui_hostCreateObject(int guiObjectType, const string objectID)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Added new object - [ %s ]", objectID.c_str());
#endif // DEBUG_GUI_SETUP

	switch ( guiObjectType )
	{
		case GUI_OBJECT_BUTTON:
			tmpGuiButton.attributes.objectID = objectID;
			tmpGuiButton.attributes.canFocus = true;
			tmpGuiButton.attributes.positionCalled = false;
			tmpGuiButton.gapSize = 20;
			tmpGuiButton.lineWidth = 3;
			tmpGuiButton.percentX = 0.15f;
			tmpGuiButton.percentY = 0.54f;
			guiButtons.push_back ( tmpGuiButton );
			break;

		case GUI_OBJECT_CHECKBOX:
			tmpGuiCheckBox.attributes.objectID = objectID;
			tmpGuiCheckBox.attributes.canFocus = true;
			tmpGuiCheckBox.attributes.positionCalled = false;
			tmpGuiCheckBox.gapSize = 8;
			tmpGuiCheckBox.lineWidth = 3;
			guiCheckBoxes.push_back (tmpGuiCheckBox);
			break;

		case GUI_OBJECT_TEXTBOX:
			tmpGuiTextBox.attributes.objectID = objectID;
			tmpGuiTextBox.attributes.canFocus = true;
			tmpGuiTextBox.attributes.positionCalled = false;
			tmpGuiTextBox.gapSize = 8;
			tmpGuiTextBox.lineWidth = 3;
			tmpGuiTextBox.contents = serverName;    // TODO Change
			guiTextBoxes.push_back (tmpGuiTextBox);
			break;

		case GUI_OBJECT_LABEL:
			tmpLabel.objectID = objectID;
			tmpLabel.canFocus = false;
			tmpLabel.positionCalled = false;
			guiLabels.push_back (tmpLabel);
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Host function called by script to set position values
void gui_hostSetObjectPosition(int guiObjectType, const string &objectID, int coordType, int startX, int startY, int width, int height)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Setting position for - [ %s ]", objectID.c_str());
#endif // DEBUG_GUI_SETUP

	int objectIndex = 0;
	//
	// Find the index for this object
	objectIndex = gui_findIndex(guiObjectType, objectID);
	if (-1 == objectIndex)
	{
		io_logToFile("ERROR: Couldn't find GUI object index [ %s ]", objectID.c_str());
		return;
	}

	switch (guiObjectType)
	{
		case GUI_OBJECT_BUTTON:
			guiButtons[objectIndex].attributes.coordType = coordType;
			if (GUI_COORD_TYPE_PERCENT == guiButtons[objectIndex].attributes.coordType)
			{
				guiButtons[objectIndex].attributes.boundingBox.w = (int)(winWidth  * ((float)width  / 100.0f));
				guiButtons[objectIndex].attributes.boundingBox.h = (int)(winHeight * ((float)height / 100.0f));

				guiButtons[objectIndex].attributes.boundingBox.x = (int)((winWidth  * ((float)startX / 100.0f)) + (guiButtons[objectIndex].attributes.boundingBox.w / 2));
				guiButtons[objectIndex].attributes.boundingBox.y = (int)((winHeight * ((float)startY / 100.0f)) + (guiButtons[objectIndex].attributes.boundingBox.h / 2));

				guiButtons[objectIndex].attributes.mouseHitBox.x = guiButtons[objectIndex].attributes.boundingBox.x - guiButtons[objectIndex].attributes.boundingBox.w;
				guiButtons[objectIndex].attributes.mouseHitBox.y = guiButtons[objectIndex].attributes.boundingBox.y;
				guiButtons[objectIndex].attributes.mouseHitBox.w = guiButtons[objectIndex].attributes.boundingBox.w;
				guiButtons[objectIndex].attributes.mouseHitBox.h = guiButtons[objectIndex].attributes.boundingBox.h;
			}
			else
			{
				guiButtons[objectIndex].attributes.boundingBox.x = startX;
				guiButtons[objectIndex].attributes.boundingBox.y = startY;
				guiButtons[objectIndex].attributes.boundingBox.w = width;
				guiButtons[objectIndex].attributes.boundingBox.h = height;

				guiButtons[objectIndex].attributes.mouseHitBox.x = startX - width;
				guiButtons[objectIndex].attributes.mouseHitBox.y = startY;  // TODO Test if height needs to be added
				guiButtons[objectIndex].attributes.mouseHitBox.w = width;
				guiButtons[objectIndex].attributes.mouseHitBox.h = height;
			}
			guiButtons[objectIndex].attributes.positionCalled = true;
		break;

		case GUI_OBJECT_CHECKBOX:
			guiCheckBoxes[objectIndex].attributes.coordType = coordType;
			if (GUI_COORD_TYPE_PERCENT == guiCheckBoxes[objectIndex].attributes.coordType)
			{
				guiCheckBoxes[objectIndex].attributes.boundingBox.w = (int)(winWidth  * ((float)width  / 100.0f));
				guiCheckBoxes[objectIndex].attributes.boundingBox.h = (int)(winHeight * ((float)height / 100.0f));

				guiCheckBoxes[objectIndex].attributes.boundingBox.x = (int)((winWidth  * ((float)startX / 100.0f))); // + (guiCheckBoxes[objectIndex].attributes.boundingBox.w / 2));
				guiCheckBoxes[objectIndex].attributes.boundingBox.y = (int)((winHeight * ((float)startY / 100.0f))); // + (guiCheckBoxes[objectIndex].attributes.boundingBox.h / 2));

				guiCheckBoxes[objectIndex].attributes.mouseHitBox.x = guiCheckBoxes[objectIndex].attributes.boundingBox.x; // + guiCheckBoxes[objectIndex].attributes.boundingBox.w;
				guiCheckBoxes[objectIndex].attributes.mouseHitBox.y = guiCheckBoxes[objectIndex].attributes.boundingBox.y;
				guiCheckBoxes[objectIndex].attributes.mouseHitBox.w = guiCheckBoxes[objectIndex].attributes.boundingBox.w;
				guiCheckBoxes[objectIndex].attributes.mouseHitBox.h = guiCheckBoxes[objectIndex].attributes.boundingBox.h;
			}
			else
			{
				guiCheckBoxes[objectIndex].attributes.boundingBox.x = startX;
				guiCheckBoxes[objectIndex].attributes.boundingBox.y = startY;
				guiCheckBoxes[objectIndex].attributes.boundingBox.w = width;
				guiCheckBoxes[objectIndex].attributes.boundingBox.h = height;

				guiCheckBoxes[objectIndex].attributes.mouseHitBox.x = startX;
				guiCheckBoxes[objectIndex].attributes.mouseHitBox.y = startY;
				guiCheckBoxes[objectIndex].attributes.mouseHitBox.w = width;
				guiCheckBoxes[objectIndex].attributes.mouseHitBox.h = height;
			}
			guiCheckBoxes[objectIndex].attributes.positionCalled = true;
			break;

		case GUI_OBJECT_TEXTBOX:
			guiTextBoxes[objectIndex].attributes.coordType = coordType;
			if (GUI_COORD_TYPE_PERCENT == guiTextBoxes[objectIndex].attributes.coordType)
			{
				guiTextBoxes[objectIndex].attributes.boundingBox.w = (int)(winWidth  * ((float)width  / 100.0f));
				guiTextBoxes[objectIndex].attributes.boundingBox.h = (int)(winHeight * ((float)height / 100.0f));

				guiTextBoxes[objectIndex].attributes.boundingBox.x = (int)((winWidth  * ((float)startX / 100.0f))); // + (guiCheckBoxes[objectIndex].attributes.boundingBox.w / 2));
				guiTextBoxes[objectIndex].attributes.boundingBox.y = (int)((winHeight * ((float)startY / 100.0f))); // + (guiCheckBoxes[objectIndex].attributes.boundingBox.h / 2));

				guiTextBoxes[objectIndex].attributes.mouseHitBox.x = guiTextBoxes[objectIndex].attributes.boundingBox.x; // + guiCheckBoxes[objectIndex].attributes.boundingBox.w;
				guiTextBoxes[objectIndex].attributes.mouseHitBox.y = guiTextBoxes[objectIndex].attributes.boundingBox.y;
				guiTextBoxes[objectIndex].attributes.mouseHitBox.w = guiTextBoxes[objectIndex].attributes.boundingBox.w;
				guiTextBoxes[objectIndex].attributes.mouseHitBox.h = guiTextBoxes[objectIndex].attributes.boundingBox.h;
			}
			else
			{
				guiTextBoxes[objectIndex].attributes.boundingBox.x = startX;
				guiTextBoxes[objectIndex].attributes.boundingBox.y = startY;
				guiTextBoxes[objectIndex].attributes.boundingBox.w = width;
				guiTextBoxes[objectIndex].attributes.boundingBox.h = height;

				guiTextBoxes[objectIndex].attributes.mouseHitBox.x = startX;
				guiTextBoxes[objectIndex].attributes.mouseHitBox.y = startY;
				guiTextBoxes[objectIndex].attributes.mouseHitBox.w = width;
				guiTextBoxes[objectIndex].attributes.mouseHitBox.h = height;
			}
			guiTextBoxes[objectIndex].attributes.positionCalled = true;
			break;

		case GUI_OBJECT_LABEL:
			guiLabels[objectIndex].coordType = coordType;
			if (GUI_COORD_TYPE_PERCENT == guiLabels[objectIndex].coordType)
			{
				guiLabels[objectIndex].boundingBox.w = (int)(winWidth  * ((float)width  / 100.0f));
				guiLabels[objectIndex].boundingBox.h = (int)(winHeight * ((float)height / 100.0f));

				guiLabels[objectIndex].boundingBox.x = (int)((winWidth  * ((float)startX / 100.0f))); // + (guiCheckBoxes[objectIndex].attributes.boundingBox.w / 2));
				guiLabels[objectIndex].boundingBox.y = (int)((winHeight * ((float)startY / 100.0f))); // + (guiCheckBoxes[objectIndex].attributes.boundingBox.h / 2));
			}
			else
			{
				guiLabels[objectIndex].boundingBox.x = startX;
				guiLabels[objectIndex].boundingBox.y = startY;
				guiLabels[objectIndex].boundingBox.w = width;
				guiLabels[objectIndex].boundingBox.h = height;
			}
			guiLabels[objectIndex].positionCalled = true;
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Set label string and position for an object
//
// Need to check that setPosition has been called before this - otherwise boundingbox isn't set
void gui_hostSetObjectLabel(int guiObjectType, const string &objectID, int labelPos, string newLabel)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Setting label for - [ %s ]", objectID.c_str());
#endif // DEBUG_GUI_SETUP

	int objectIndex = 0;
	//
	// Find the index for this object
	objectIndex = gui_findIndex ( guiObjectType, objectID );
	if ( -1 == objectIndex )
	{
		io_logToFile ( "ERROR: Couldn't find GUI object index [ %s ]", objectID.c_str ());
		return;
	}

	switch ( guiObjectType )
	{
		case GUI_OBJECT_BUTTON:
			if (!guiButtons[objectIndex].attributes.positionCalled)
			{
				con_print(CON_ERROR, true, "GUI object position has not been set [ %s ]", objectID.c_str());
				return;
			}
			guiButtons[objectIndex].attributes.label = std::move ( newLabel ); // TODO check what this does
			guiButtons[objectIndex].attributes.labelPos = labelPos;
			break;

		case GUI_OBJECT_CHECKBOX:
			if (!guiCheckBoxes[objectIndex].attributes.positionCalled)
			{
				con_print(CON_ERROR, true, "GUI object position has not been set [ %s ]", objectID.c_str());
				return;
			}
			guiCheckBoxes[objectIndex].attributes.label = std::move ( newLabel ); // TODO check what this does
			guiCheckBoxes[objectIndex].attributes.labelPos = labelPos;
			break;

		case GUI_OBJECT_TEXTBOX:
			if (!guiTextBoxes[objectIndex].attributes.positionCalled)
			{
				con_print(CON_ERROR, true, "GUI object position has not been set [ %s ]", objectID.c_str());
				return;
			}
			guiTextBoxes[objectIndex].attributes.label = std::move ( newLabel ); // TODO check what this does
			guiTextBoxes[objectIndex].attributes.labelPos = labelPos;
			break;

		case GUI_OBJECT_LABEL:
			if (!guiLabels[objectIndex].positionCalled)
			{
				con_print(CON_ERROR, true, "GUI object position has not been set [ %s ]", objectID.c_str());
				return;
			}
			guiLabels[objectIndex].label = std::move ( newLabel ); // TODO check what this does
			guiLabels[objectIndex].labelPos = labelPos;
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Create a new GUI screen which will contain other GUI objects
void gui_hostCreateNewScreen(string screenID)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Screen [ %s ] created.", screenID.c_str());
#endif // DEBUG_GUI_SETUP

	tmpScreen.screenID = screenID;
	tmpScreen.selectedObject = 0;      // default to first object on screen having focus

	guiScreens.push_back(tmpScreen);
}

//-----------------------------------------------------------------------------
//
// Associate the object to a screen, recording it's index and type
void gui_hostAddObjectToScreen(int guiObjectType, string objectID, string whichScreen)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Adding [ %s ] to screen [ %s ]", objectID.c_str(), whichScreen.c_str());
#endif // DEBUG_GUI_SETUP

	int screenIndex = 0;
	int objectIndex = 0;
	//
	// Find the index for this screen
	screenIndex = gui_findIndex(GUI_OBJECT_SCREEN, whichScreen);
	if (-1 == screenIndex)
	{
		io_logToFile("ERROR: Couldn't find GUI screen index [ %s ]", whichScreen.c_str());
		return;
	}
	//
	// Find the index for this object
	objectIndex = gui_findIndex(guiObjectType, objectID);
	if (-1 == objectIndex)
	{
		io_logToFile("ERROR: Couldn't find GUI object index [ %s ]", objectID.c_str());
		return;
	}

	switch (guiObjectType)
	{
		case GUI_OBJECT_BUTTON:
		case GUI_OBJECT_CHECKBOX:
		case GUI_OBJECT_TEXTBOX:
		case GUI_OBJECT_LABEL:
//		case GUI_OBJECT_IMAGE:
//		case GUI_OBJECT_SLIDER:
//		case GUI_OBJECT_KEYCODE:
			guiScreens[screenIndex].objectIDIndex.push_back(objectIndex);    // Add objectIndex to the end
			guiScreens[screenIndex].objectType.push_back(guiObjectType);

#ifdef DEBUG_GUI_SETUP
		io_logToFile("Screen [ %s ] objectID [ %i ] - Added", guiScreens[screenIndex].screenID.c_str(), guiScreens[screenIndex].objectIDIndex.back());
				io_logToFile("-------------");
#endif
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Map four Uint8 to a SDL_Color structure
SDL_Color gui_mapRGBA(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
//-----------------------------------------------------------------------------
{
	SDL_Color   returnValue;

	returnValue.r = red;
	returnValue.g = green;
	returnValue.b = blue;
	returnValue.a = alpha;

	return returnValue;
}

//-----------------------------------------------------------------------------
//
// Set the color for the passed in element for this object
//
// Colors are in range 0..255
void gui_setObjectColorByIndex(int guiObjectType, int objectIndex, int whichColor, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
//-----------------------------------------------------------------------------
{
	switch (guiObjectType)
	{
		//
		// Colors for guiButton
		case GUI_OBJECT_BUTTON:
			if (objectIndex > guiButtons.size() - 1)
			{
				con_print(CON_ERROR, true, "Index used to access guiButtons is too large.");
				return;
			}

			switch ( whichColor )
			{
				case GUI_ACTIVE_COL:
					guiButtons[objectIndex].attributes.hasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_COL:
					guiButtons[objectIndex].attributes.noFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_CORNER_COL:
					guiButtons[objectIndex].cornerFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_CORNER_COL:
					guiButtons[objectIndex].cornerNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_LABEL_COL:
					guiButtons[objectIndex].attributes.labelHasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_LABEL_COL:
					guiButtons[objectIndex].attributes.labelNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				default:
					break;
			}
			break;

		case GUI_OBJECT_CHECKBOX:
			if (objectIndex > guiCheckBoxes.size() - 1)
			{
				con_print(CON_ERROR, true, "Index used to access guiButtons is too large.");
				return;
			}

			switch ( whichColor )
			{
				case GUI_ACTIVE_COL:
					guiCheckBoxes[objectIndex].attributes.hasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_COL:
					guiCheckBoxes[objectIndex].attributes.noFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_CORNER_COL:
					guiCheckBoxes[objectIndex].cornerFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_CORNER_COL:
					guiCheckBoxes[objectIndex].cornerNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_LABEL_COL:
					guiCheckBoxes[objectIndex].attributes.labelHasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_LABEL_COL:
					guiCheckBoxes[objectIndex].attributes.labelNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				default:
					break;
			}
			break;

		case GUI_OBJECT_TEXTBOX:
			if (objectIndex > guiTextBoxes.size() - 1)
			{
				con_print(CON_ERROR, true, "Index used to access guiButtons is too large.");
				return;
			}

			switch ( whichColor )
			{
				case GUI_ACTIVE_COL:
					guiTextBoxes[objectIndex].attributes.hasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_COL:
					guiTextBoxes[objectIndex].attributes.noFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_CORNER_COL:
					guiTextBoxes[objectIndex].cornerFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_CORNER_COL:
					guiTextBoxes[objectIndex].cornerNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_LABEL_COL:
					guiTextBoxes[objectIndex].attributes.labelHasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_LABEL_COL:
					guiTextBoxes[objectIndex].attributes.labelNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				default:
					break;
			}
			break;

		case GUI_OBJECT_LABEL:
			if (objectIndex > guiLabels.size() - 1)
			{
				con_print(CON_ERROR, true, "Index used to access guiButtons is too large.");
				return;
			}

			switch ( whichColor )
			{
				case GUI_ACTIVE_COL:
					guiLabels[objectIndex].hasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_COL:
					guiLabels[objectIndex].noFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_ACTIVE_LABEL_COL:
					guiLabels[objectIndex].labelHasFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				case GUI_INACTIVE_LABEL_COL:
					guiLabels[objectIndex].labelNoFocusColor = gui_mapRGBA ( red, green, blue, alpha );
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Host function called by script to set object colors
void gui_hostSetObjectColor(int guiObjectType, string objectID, int whichColor, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Setting colors for - [ %s ]", objectID.c_str());
#endif // DEBUG_GUI_SETUP

	int objectIndex = 0;
	unsigned long	numObjects = 0;

	if (objectID == "ALL")	// Are we setting the colors for all of this type of object
	{
		switch (guiObjectType)	// Find out how many of this type there are
		{
			case GUI_OBJECT_BUTTON:
				numObjects = guiButtons.size();
				break;

			case GUI_OBJECT_CHECKBOX:
				numObjects = guiCheckBoxes.size();
				break;

			case GUI_OBJECT_TEXTBOX:
				numObjects = guiTextBoxes.size();
				break;

			case GUI_OBJECT_LABEL:
				numObjects = guiLabels.size();
				break;

			default:
				break;
		}

		if (numObjects == 0)
			return;

		for (int i = 0; i < numObjects; i++)	// Loop through each object and set it's color
			gui_setObjectColorByIndex(guiObjectType, i, whichColor, red, green, blue, alpha);
	}
	else    // Just setting color for one object
	{
		//
		// Find the index for this object
		objectIndex = gui_findIndex(guiObjectType, objectID);
		if (-1 == objectIndex)
		{
			con_print(CON_ERROR, true, "ERROR: Couldn't find GUI object index [ %s ]", objectID.c_str());
			return;
		}
		gui_setObjectColorByIndex(guiObjectType, objectIndex, whichColor, red, green, blue, alpha);
	}
}


//-----------------------------------------------------------------------------
//
// Set the script function to run when actioned
void gui_hostSetObjectFunctions(int guiObjectType, string objectID, string clickFunction)
//-----------------------------------------------------------------------------
{
#ifdef DEBUG_GUI_SETUP
	io_logToFile("Setting the function call for [ %s ] - [ %s ]", objectID.c_str(), clickFunction.c_str());
#endif // DEBUG_GUI_SETUP

	int objectIndex = 0;
	//
	// Find the index for this object
	objectIndex = gui_findIndex(guiObjectType, objectID);
	if (-1 == objectIndex)
	{
		io_logToFile("ERROR: Couldn't find GUI object index [ %s ]", objectID.c_str());
		return;
	}

	switch (guiObjectType)
	{
		//
		// Actions for a object
		case GUI_OBJECT_BUTTON:
			guiButtons[objectIndex].attributes.action = clickFunction;
			break;

		case GUI_OBJECT_CHECKBOX:
			guiCheckBoxes[objectIndex].attributes.action = clickFunction;
			break;

		case GUI_OBJECT_TEXTBOX:
			guiTextBoxes[objectIndex].attributes.action = clickFunction;
			break;

		default:
			break;
	}
}

//--------------------------------------------------------------------------
//
// Draw the GUI for the current screen
//
void gui_displayGUI()
//--------------------------------------------------------------------------
{
	float texCoords[] = {
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0};  // Y is reversed for this texture

	if (!guiReady)
		return;

	glViewport (0, 0, winWidth, winHeight);
	gl_set2DMode(glm::vec2{0,0}, glm::vec2{winWidth,winHeight}, glm::vec3(1, 1, 1));

	switch (currentMode)
	{
		case MODE_GUI:
			gui_drawGUI ();
			break;

		case MODE_INTRO:
			gui_drawScrollBox ( &introScrollBox );
			break;

		default:
			break;
	}

	//
	// Upload the SDL Surface into a OpenGL texture
	gui_surfaceToGL(guiSurface, "guiScreen");

	glm::vec2 guiPosition;
	glm::vec2 guiSize;
	glm::vec3 guiColorKey;
	glm::vec3 guiTintColor;

	guiPosition.x = 0.0f;
	guiPosition.y = 0.0f;

	guiSize.x = winWidth;
	guiSize.y = winHeight;

	guiColorKey.r = 1.0f;
	guiColorKey.g = 0.0f;
	guiColorKey.b = 0.0f;

	guiTintColor.r = -1.0f;
	guiTintColor.g = 0.0f;
	guiTintColor.b = 1.0f;
	//
	// Put the OpenGL texture onto the visible framebuffer
	gl_draw2DQuad ( guiPosition, guiSize, "colorKey", io_getTextureID ("guiScreen"), guiColorKey, guiTintColor, texCoords);

	s_renderHUD ();

	// Select the color for clearing the renderer
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	/* Clear the entire screen to our selected color. */
	SDL_RenderClear(renderer);

}

//--------------------------------------------------------------------------
//
// Create a SDL Surface and convert into a OpenGL texture
// The OpenGL texture is also recorded in the texture map array for accessing later
void gui_surfaceToGL(SDL_Surface *whichSurface, string textureName)
//--------------------------------------------------------------------------
{
	static Uint32       newTextureID = 0;
	glm::vec2           newImageSize;

	if (!guiReady)
		return;

	if (0 == newTextureID)
		glGenTextures(1, &newTextureID);

	glBindTexture(GL_TEXTURE_2D, newTextureID);

	int Mode = GL_RGB;

	if (whichSurface->format->BytesPerPixel == 4)
	{
		Mode = GL_RGBA;
	}

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_LockSurface(whichSurface);
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, whichSurface->w, whichSurface->h, 0, Mode, GL_UNSIGNED_BYTE, whichSurface->pixels);
	SDL_UnlockSurface (whichSurface);

	newImageSize.x = whichSurface->w;
	newImageSize.y = whichSurface->h;

	io_storeTextureInfoIntoMap(newTextureID, newImageSize, textureName, false);
}

//--------------------------------------------------------------------------
//
// Prepare the GUI - call scripts, setup scrollbox values
// Needs to happen after GUI fonts are loaded
void gui_prepareGUI()
//--------------------------------------------------------------------------
{
	gui_setFontName ( "fontDigital32" );

	con_executeScriptFunction ( "scr_setupGUI", "" );

	gui_setupScrollBox ( SCROLLBOX_INTRO, &introScrollBox, "scrollText" );

	guiReady = true;
}

//--------------------------------------------------------------------------
//
// Create the GUI SDL_Surface, which is the destination surface to hold
// all the SDL GUI rendering - gets uploaded to GL texture for quad render
bool gui_initGUI()
//--------------------------------------------------------------------------
{
	Uint32              rmask, gmask, bmask, amask;

	// SDL interprets each pixel as a 32-bit number, so our masks must depend
    // on the endianness (byte order) of the machine

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	printf("About to initGUI.\n");

	if (nullptr == guiSurface)
	{
		guiSurface = SDL_CreateRGBSurface ( 0, winWidth, winHeight, 32, rmask, gmask, bmask, amask );
		if ( guiSurface == nullptr )
		{
			con_print ( CON_ERROR, true, "Failed to create internal SDL_Surface." );
			return false;
		}

		con_print ( CON_INFO, true, "SDL_Surface destination created." );
	}

	if (nullptr == renderer)
	{
		//
		// Create a software renderer pointing to our surface
		renderer = SDL_CreateSoftwareRenderer ( guiSurface );
		if ( !renderer )
		{
			con_print ( CON_ERROR, true, "Render creation for surface failed [ %s ]", SDL_GetError ());
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------
//
// Set the name of the font to be used
void gui_setFontName(std::string newFontName)
//--------------------------------------------------------------------------
{
	guiFontName = std::move ( newFontName );
}

//--------------------------------------------------------------------------
//
// Handle the event of an error with the TTF loading
void gui_handleTTFError( const int errorCode, const string fileName )
//--------------------------------------------------------------------------
{
	switch (errorCode)
	{
		case LEVEL_LOAD_ERROR_NOT_FOUND:
			con_print ( CON_ERROR, true, "Could not find TTF file [ %s ]", fileName.c_str ());
			break;

		case LEVEL_LOAD_MALLOC_ERROR:
			con_print ( CON_ERROR, true, "Memory loading error for [ %s ]", fileName.c_str ());
			break;

		case USER_EVENT_TTF_BAD_LOAD:
			con_print( CON_ERROR, true, "Error loading font [ %s ]", fileName.c_str());
			break;

		case LEVEL_LOAD_ERROR_FILESYSTEM:
			con_print(CON_ERROR, true, "File system is not reading to load font [ %s ]", fileName.c_str());
			break;

		case USER_EVENT_TTF_INIT_FAIL:
			con_print(CON_ERROR, true, "Unable to start TTF system [ %s ]", fileName.c_str());
			break;

		case LEVEL_LOAD_MEMORY_ERROR:
			con_print(CON_ERROR, true, "Could not load file into memory [ %s ]", fileName.c_str());
			break;

		default:
			break;
	}
}

//--------------------------------------------------------------------------
//
// CLose font system
void gui_closeFontSystem()
//--------------------------------------------------------------------------
{
	for (auto itr : ttfFonts)
	{
		free(itr.fontFileMemPtr);
		TTF_CloseFont (itr.ttfFont);
		itr.ttfFont = nullptr;
	}
}

//--------------------------------------------------------------------------
//
// Load a TTF from the packfile
bool gui_loadTTFFont(string fileName, int fontSize, string indexName)
//--------------------------------------------------------------------------
{
	SDL_RWops       *fp;
	PHYSFS_sint64   fontFileMemSize;
	_font           tempTTFFont;
	int             fontWidth, fontHeight;

	//
	// Start the library once before using it
	if (!TTF_WasInit() && TTF_Init() == -1)
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_TTF_ERROR, USER_EVENT_TTF_INIT_FAIL, 0, 0, vec2 (), vec2 (), TTF_GetError());
		return false;
	}

	if (!io_isFileSystemReady ())
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_TTF_ERROR, LEVEL_LOAD_ERROR_FILESYSTEM, 0, 0, vec2 (), vec2 (), fileName);
		return false;   // TODO: Start file system??
	}

	if (!io_doesFileExist(fileName))
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_TTF_ERROR, LEVEL_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), fileName);
		return false;
	}

	fontFileMemSize = io_getFileSize ( fileName.c_str());

	tempTTFFont.fontFileMemPtr = (int *) malloc (sizeof (int) * fontFileMemSize);
	if ( nullptr == tempTTFFont.fontFileMemPtr )
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_TTF_ERROR, LEVEL_LOAD_MALLOC_ERROR, 0, 0, vec2 (), vec2 (), fileName);
		return false;
	}

	if ( -1 == io_getFileIntoMemory (fileName.c_str(), tempTTFFont.fontFileMemPtr))
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_TTF_ERROR, LEVEL_LOAD_MEMORY_ERROR, 0, 0, vec2 (), vec2 (), fileName);
		free(tempTTFFont.fontFileMemPtr);
		tempTTFFont.fontFileMemPtr = nullptr;
		return false;
	}

	fp = SDL_RWFromMem( tempTTFFont.fontFileMemPtr, static_cast<int>(fontFileMemSize));

	tempTTFFont.ttfFont = TTF_OpenFontRW(fp, 0, fontSize);
	if (!tempTTFFont.ttfFont)
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_TTF_ERROR, USER_EVENT_TTF_BAD_LOAD, 0, 0, vec2 (), vec2 (), TTF_GetError());
		return false;
	}

	tempTTFFont.valid = true;
	tempTTFFont.fileName = fileName;
	tempTTFFont.fontSize = fontSize;

	TTF_SizeText(tempTTFFont.ttfFont, "ABCqQjJiIpP", &fontWidth, &fontHeight);
	tempTTFFont.fontHeight = fontHeight;            // store the font height

	ttfFonts.push_back(tempTTFFont);

	ttfFontIndex.insert(std::pair<string, int>(indexName, ttfFonts.size() - 1));

	return true;
}

//--------------------------------------------------------------------------
//
// Return the index for the font based on the passed in fontIndexName
unsigned int gui_getFontIndex(std::string fontIndexName)
//--------------------------------------------------------------------------
{
	unsigned int        fontIndex;

	auto itr = ttfFontIndex.find ( fontIndexName );

	if ( itr != ttfFontIndex.end ())    // Found
		{
		return itr->second;
		}
	con_print(CON_ERROR, true, "Unable to find index for font [ %s ]", fontIndexName.c_str());

	return 255;
}

//--------------------------------------------------------------------------
//
// Render a text string using a TTF font
//
// Pass in destination SDL_Surface to be used
bool gui_renderText( string fontIndexName, glm::vec2 position, glm::vec3 color, SDL_Surface *destSurface, std::string textString, ...)
//--------------------------------------------------------------------------
{
	unsigned int            fontIndex;
	static int              errorCountNotFound = 0; // Prevent spamming the console
	static int              errorCountRender = 0;   // Prevent spamming the console
	static int              errorCountValid = 0;    // Prevent spamming the console
	SDL_Surface             *fontSurface = nullptr;
	SDL_Color               fontColor;
	SDL_Rect                destRect;

	va_list                 args, args_copy;

	if (!guiReady)
		return false;

	fontIndex = gui_getFontIndex (fontIndexName);

	if ( fontIndex != 255 )    // Found
	{
		if (!ttfFonts[fontIndex].valid)
		{
			if (errorCountValid < 3)
			{
				errorCountValid++;
				con_print ( CON_ERROR, true, "Attempting to use an invalid font [ %s ]", fontIndexName.c_str() );
				return false;
			}
		}

		fontColor.r = static_cast<Uint8>(color.x);
		fontColor.g = static_cast<Uint8>(color.y);
		fontColor.b = static_cast<Uint8>(color.z);

		va_start( args, textString );
		va_copy( args_copy, args );

		const auto sz = std::vsnprintf ( nullptr, 0, textString.c_str (), args ) + 1;

		try // Get the paramaters into the final string 'result'
		{
			std::string result ( sz, ' ' );
			std::vsnprintf ( &result.front (), sz, textString.c_str (), args_copy );

			va_end(args_copy);
			va_end(args);

			fontSurface = TTF_RenderText_Solid ( ttfFonts[fontIndex].ttfFont, result.c_str (), fontColor );
			if ( !fontSurface )
			{
				if ( errorCountRender < 3 )
				{
					con_print ( CON_ERROR, true, "TTF Font render error [ %s ]", TTF_GetError ());
					errorCountRender++;
					return false;
				}
				return false;
			}
			//
			// Everything worked - blit the surface with the font on it, to the destination surface
			destRect.x = static_cast<int>(position.x);
			destRect.y = static_cast<int>(position.y);
			destRect.w = fontSurface->w;
			destRect.h = fontSurface->h;
			SDL_BlitSurface ( fontSurface, nullptr, destSurface, &destRect );

			SDL_FreeSurface ( fontSurface );
			return true;
		}

		catch ( const std::bad_alloc& )
		{
			va_end(args_copy);
			va_end(args);
			printf("Error allocating string parsing in con_print.\n");
			return false;
		}
	}

	//
	// Could not find font index
	if (errorCountNotFound < 3)
	{
		con_print ( CON_ERROR, true, "Unable to find fontName [ %s ]", fontIndexName.c_str ());
		errorCountNotFound++;
	}
	return false;

}


//-----------------------------------------------------------------------------
//
// Return if an object can be selected or not
bool gui_canObjectBeSelected(int objectType)
//-----------------------------------------------------------------------------
{
	switch (objectType)
	{
		case GUI_OBJECT_BUTTON:
		case GUI_OBJECT_CHECKBOX:
		case GUI_OBJECT_TEXTBOX:
			return true;

		case GUI_OBJECT_LABEL:
			return false;

		default:
			break;
	}
	return false;
}

//-----------------------------------------------------------------------------
//
// Move focus to next object
void gui_handleFocusMove(int moveDirection, bool takeAction, int eventSource)
//-----------------------------------------------------------------------------
{
	int             indexCount = 0;
	int             selectedSlider, selectedKeyCode;
	SDL_Point       mouseLocation;

	indexCount = 0;

	selectedSlider = guiScreens[currentGUIScreen].objectIDIndex[guiScreens[currentGUIScreen].selectedObject];
	selectedKeyCode = guiScreens[currentGUIScreen].objectIDIndex[guiScreens[currentGUIScreen].selectedObject];

	mouseLocation = io_getMousePointLocation ();

	if ( takeAction )
	{
		currentObjectSelected = guiScreens[currentGUIScreen].objectIDIndex[guiScreens[currentGUIScreen].selectedObject];

		switch (guiScreens[currentGUIScreen].objectType[guiScreens[currentGUIScreen].selectedObject])
		{
			case GUI_OBJECT_BUTTON:
				if (!eventSource)   // not a mouse action
					con_executeScriptFunction(guiButtons[currentObjectSelected].attributes.action, guiButtons[currentObjectSelected].attributes.objectID);
				else
				{   // Event is from mouse - check it's inside the bounds of this button to be activated
					if (SDL_PointInRect(&mouseLocation, &guiButtons[currentObjectSelected].attributes.mouseHitBox))
						con_executeScriptFunction(guiButtons[currentObjectSelected].attributes.action, guiButtons[currentObjectSelected].attributes.objectID);
				}
				break;

			case GUI_OBJECT_CHECKBOX:
				if (!eventSource)
					con_executeScriptFunction (guiCheckBoxes[currentObjectSelected].attributes.action, guiCheckBoxes[currentObjectSelected].attributes.objectID);
				else
				{
					if (SDL_PointInRect (&mouseLocation, &guiCheckBoxes[currentObjectSelected].attributes.mouseHitBox))
						con_executeScriptFunction (guiCheckBoxes[currentObjectSelected].attributes.action, guiCheckBoxes[currentObjectSelected].attributes.objectID);
				}

			default:
				break;
		}
		return;
	}

	switch (moveDirection)
	{
		case FOCUS_NEXT:
			indexCount = 1;
			if (guiScreens[currentGUIScreen].selectedObject != (int)guiScreens[currentGUIScreen].objectIDIndex.size() - 1)	// Don't go past number of objects on the screen
			{
				while ( !gui_canObjectBeSelected( guiScreens[currentGUIScreen].objectType[guiScreens[currentGUIScreen].selectedObject + indexCount]))
				{
					indexCount++;
				}
				guiScreens[currentGUIScreen].selectedObject += indexCount;

				if (indexCount > (int)guiScreens[currentGUIScreen].objectIDIndex.size())
				{
					indexCount = static_cast<int>(guiScreens[currentGUIScreen].objectIDIndex.size());
				}

				currentObjectSelected = guiScreens[currentGUIScreen].selectedObject;
			}
			break;

		case FOCUS_PREVIOUS:
			indexCount = 1;
			if (guiScreens[currentGUIScreen].selectedObject > 0)
			{
				while ( !gui_canObjectBeSelected( guiScreens[currentGUIScreen].objectType[guiScreens[currentGUIScreen].selectedObject - indexCount]))
				{
					indexCount++;
					if (guiScreens[currentGUIScreen].selectedObject - indexCount < 0)
						return;
				}
				guiScreens[currentGUIScreen].selectedObject -= indexCount;

				if (guiScreens[currentGUIScreen].selectedObject < 0)
					guiScreens[currentGUIScreen].selectedObject = 0;

				currentObjectSelected = guiScreens[currentGUIScreen].selectedObject;
			}
			break;

		case FOCUS_LEFT:
			switch (guiScreens[currentGUIScreen].objectType[guiScreens[currentGUIScreen].selectedObject])
			{
			}
			break;

		case FOCUS_RIGHT:
			switch (guiScreens[currentGUIScreen].objectType[guiScreens[currentGUIScreen].selectedObject])
			{
			}
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Handle an input event for a GUI object
void gui_handleInputEvent(int eventAction, int eventType, int eventSource)
//-----------------------------------------------------------------------------
{
	std::string tempString;

	if (eventAction == MY_INPUT_ACTION_RELEASE)
	{
		if (currentMode == MODE_INTRO)
		{
			evt_removeTimer (introScrollPauseTimerID);
			evt_removeTimer (introScrollTimerID);
			sys_changeMode(MODE_GUI);
			return;
		}

		switch ( eventType )
		{
			case MY_INPUT_UP:
				// Play sound
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_GOOD, 0, 0, glm::vec2 (), glm::vec2 (), "" );
				gui_handleFocusMove ( FOCUS_PREVIOUS, false, eventSource );
				break;

			case MY_INPUT_DOWN:
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_GOOD, 0, 0, glm::vec2 (), glm::vec2 (), "" );
				gui_handleFocusMove ( FOCUS_NEXT, false, eventSource );
				break;

			case MY_INPUT_ACTION:
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_GOOD, 0, 0, glm::vec2 (), glm::vec2 (), "" );
				gui_handleFocusMove ( -1, true, eventSource );  //eventSource is TRUE if event is from mouse
				break;

			default:
				if (GUI_OBJECT_TEXTBOX == guiScreens[currentGUIScreen].objectType[guiScreens[currentGUIScreen].selectedObject])
				{
					if (eventType == SDLK_BACKSPACE)
					{
						guiTextBoxes[guiScreens[currentGUIScreen].objectIDIndex[guiScreens[currentGUIScreen].selectedObject]].contents.clear();
						return;
					}
					guiTextBoxes[guiScreens[currentGUIScreen].objectIDIndex[guiScreens[currentGUIScreen].selectedObject]].contents += eventType;
				}
				break;
		}
	}
	else
	{
		switch (eventType)
		{
			case MY_INPUT_CONSOLE:
				sys_changeMode (MODE_CONSOLE);
				break;

		}
	}
}

//-----------------------------------------------------------------------------
//
// Handle a mouse motion event on a GUI screen
void gui_handleMouseMotion(glm::vec2 mousePosition)
//-----------------------------------------------------------------------------
{
	SDL_Point       mousePoint;

	//
	// Draw GUI object elements
	int indexCount = 0;

	if (!guiReady)
		return;

	mousePoint.x = static_cast<int>(mousePosition.x);
	mousePoint.y = static_cast<int>(mousePosition.y);

	for ( auto it = guiScreens[currentGUIScreen].objectIDIndex.begin (); it != guiScreens[currentGUIScreen].objectIDIndex.end (); ++it )
	{
		switch (guiScreens[currentGUIScreen].objectType[indexCount])
		{
			case GUI_OBJECT_BUTTON:
				if (SDL_PointInRect(&mousePoint, &guiButtons[guiScreens[currentGUIScreen].objectIDIndex[indexCount]].attributes.mouseHitBox))
				{
					guiScreens[currentGUIScreen].selectedObject = indexCount;
				}
				break;

			case GUI_OBJECT_CHECKBOX:
				if (SDL_PointInRect (&mousePoint, &guiCheckBoxes[guiScreens[currentGUIScreen].objectIDIndex[indexCount]].attributes.mouseHitBox))
				{
					guiScreens[currentGUIScreen].selectedObject = indexCount;
				}
				break;

			case GUI_OBJECT_TEXTBOX:
				if (SDL_PointInRect (&mousePoint, &guiTextBoxes[guiScreens[currentGUIScreen].objectIDIndex[indexCount]].attributes.mouseHitBox))
				{
					guiScreens[currentGUIScreen].selectedObject = indexCount;
				}
				break;

			default:
				break;
		}
		indexCount++;
	}
}
