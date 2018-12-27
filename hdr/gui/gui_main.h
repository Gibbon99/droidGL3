#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/libGL/gl_window.h"
#include "hdr/gui/SDL_ttf.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/libGL/sdl2_gfx/SDL2_gfxPrimitives.h"


// All GUI elements are added to their respective arrays.  The index for a particular element is then stored in the 'screen' array.
//
// Each Screen is comprised of a name, and indexes into each of the arrays for each type of element
// Each screen is drawn using SDL_gfx and SDL_ttf, then the surface ( same size as window ) is uploaded to OpenGL
// as a single texture.  All animation and effects are done on the SDL_Surface version - it is recreated each frame.
//

#define FOCUS_NEXT						0
#define FOCUS_PREVIOUS					1
#define FOCUS_LEFT						2
#define FOCUS_RIGHT						3

extern bool                                    guiReady;
extern SDL_Renderer                           *renderer;       // Global for all GUI functions to access
extern SDL_Surface                            *guiSurface;     // GLobal surface where all SDL rendering takes place
extern std::string                             guiFontName;     // Which font is currently in use
extern int                                     currentObjectSelected;  // Pass this to script to act on
extern SDL_Color                               focusAnimateColor;

typedef struct
{
	TTF_Font		   *ttfFont;
	bool				valid;
	string				fileName;
	int                 fontSize;
	int                 fontHeight;
	int                *fontFileMemPtr;
} _font;

extern vector<_font>        ttfFonts;

typedef struct
{
	int              	selectedObject;
	string           	screenID;          		// Name of this screen
	vector<int>     	objectIDIndex;          // Index into object array
	vector<int>    		objectType;        		// Which object array
} _screenObject;

typedef struct
{
	string             	objectID;       // Unique for each type
	bool                canFocus;       // Can receive focus or not
	bool                positionCalled; // Has this object had it's position set; boundingBox values are populated?
	int             	objectType;     // Button, checkBox etc
	int             	screenID;       // Which screen will this control be drawn on
	int             	coordType;      // Method to work out positioning
	int              	labelPos;       // Where to place labels on buttons
	string            	label;          // Text associated with control
	string              action;         // Run this when activated
	SDL_Rect          	boundingBox;    // Drawing bounds
	SDL_Rect            mouseHitBox;    // used for mouse detection and drawing position
	SDL_Color           noFocusColor;   // Normal unselected color
	SDL_Color           hasFocusColor;  // Highlighted color
	SDL_Color           labelNoFocusColor;
	SDL_Color           labelHasFocusColor;
} _guiObject;

typedef struct
{
	_guiObject          attributes;     // Common to all objects
	SDL_Color           cornerNoFocusColor;    // Color for the GGE
	SDL_Color           cornerFocusColor;       // Color for the corner element
	int                 lineWidth;      // How think to draw the outline
	int                 gapSize;        // Space between button and corner element
	float               percentX;       // How far along to start the corner cutout
	float               percentY;       // How far along to start the corner cutout
} _guiButton;

typedef struct
{
	_guiObject          attributes;     // Common to all objects
	bool                checked;        // Is the checkbox set
	int                 gapSize;        // Space between edge of box and start of text
	int                 lineWidth;      // Thickness of box outline
	SDL_Color           cornerFocusColor;       // Color for the check mark
	SDL_Color           cornerNoFocusColor;    // Color for the check mark with no focus on object
} _guiCheckBox;

typedef struct
{
	_guiObject          attributes;     // Common to all objects
	int                 gapSize;        // Space between label and editbox
	int                 lineWidth;      // Thickness of box outline
	SDL_Color           cornerFocusColor;       // Color for the check mark
	SDL_Color           cornerNoFocusColor;    // Color for the check mark with no focus on object
	string              contents;       // Text in the editbox
} _guiTextBox;

extern int				        currentGUIScreen;
extern vector<_screenObject>	guiScreens;
extern vector<_guiButton>       guiButtons;
extern vector<_guiCheckBox>     guiCheckBoxes;
extern vector<_guiTextBox>      guiTextBoxes;
extern vector<_guiObject>       guiLabels;

// Handle a GUI event - called by thread
int gam_processGuiEventQueue ( void *ptr );

// Draw a gui button using a texture
void gui_displayGUI();

// Create a SDL Surface and convert into a OpenGL texture
// The OpenGL texture is also recorded in the texture map array for accessing later
void gui_surfaceToGL(SDL_Surface *whichSurface, string textureName);

// Handle the event of an error with the TTF loading
void gui_handleTTFError( const int errorCode, const string fileName );

// CLose font system
void gui_closeFontSystem();

// Pass in destination SDL_Surface to be used
bool gui_renderText(string fontIndexName, glm::vec2 position, glm::vec3 color, SDL_Surface *destSurface, string textString, ...);

// Create the GUI SDL_Surface, which is the destination surface to hold
// all the SDL GUI rendering - gets uploaded to GL texture for quad render
bool gui_initGUI();

// Prepare the GUI - call scripts, setup scrollbox values
// Needs to happen after GUI fonts are loaded
void gui_prepareGUI();

bool gui_loadTTFFont(string fileName, int fontSize, string indexName);

// Return the index for the font based on the passed in fontIndexName
unsigned int gui_getFontIndex(string fontIndexName);

// Set the name of the font to be used
void gui_setFontName(string newFontName);

// Map four Uint8 to a SDL_Color structure
SDL_Color gui_mapRGBA(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);



// Add a gui object to it's relevant vector array
void gui_hostCreateObject(int guiObjectType, const string objectID);

// Host function called by script to set position values
void gui_hostSetObjectPosition(int guiObjectType, const string &objectID, int coordType, int startX, int startY, int width, int height);

// Set label string and position for an object
void gui_hostSetObjectLabel(int guiObjectType, const string &objectID, int labelPos, string newLabel);

// Create a new GUI screen which will contain other GUI objects
void gui_hostCreateNewScreen(string screenID);

// Associate the object to a screen, recording it's index and type
void gui_hostAddObjectToScreen(int guiObjectType, string objectID, string whichScreen);

// Host function called by script to set object colors
void gui_hostSetObjectColor(int guiObjectType, string objectID, int whichColor, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

// Set the script function to run when clicked or mouse is over
void gui_hostSetObjectFunctions(int guiObjectType, string objectID, string clickFunction);

// Find the objectID on the current screen and make it selected
void gui_setObjectFocus(string objectID);

// Look through the relevant vector to locate the index of the objectID
//
// returns index value, or -1 if not found
int gui_findIndex(int guiObjectType, const string objectID);

// Move focus to next object
void gui_handleFocusMove(int moveDirection, bool takeAction, int eventSource);

// Handle a mouse motion event on a GUI screen
void gui_handleMouseMotion(glm::vec2 mousePosition);

// Handle an input event for a GUI object
void gui_handleInputEvent(int eventAction, int eventType, int eventSource);
