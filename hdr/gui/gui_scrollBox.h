#pragma once

#include "hdr/gui/gui_main.h"
#include "hdr/gui/gui_button.h"

#define SCROLLBOX_INTRO     1
#define SCROLLBOX_DB        2

typedef struct
{
	int         	startX;
	int         	startY;
	int         	width;
	int         	height;
//	int         	whichFont;
	SDL_Color    	backGroundCol;
	SDL_Color    	fontCol;
	int             fontLineHeight;             // How tall is the font - fine scroll this much
	int         	linesToPrint;               // how far up have we scrolled
	int             numVisibleLinesHeight;      // How many lines can be displayed at once
	int         	charPtr;                    // which character are we up to
	float       	scrollY;		            // How many fine pixel lines scrolled
	Uint32       	scrollSpeed;
	float       	scrollDelayCount;
	float		    scrollDelay;	// Adjustable by text
	vector<string>  textLine;
	string			sourceText;
} _scrollBox;

extern _scrollBox		introScrollBox;
extern _scrollBox   	databaseScrollBox;

extern SDL_TimerID         introScrollTimerID;
extern SDL_TimerID         introScrollPauseTimerID;

extern int introStartX;
extern int introStartY;
extern int introWidth;
extern int introHeight;
extern int introBackRed;
extern int introBackGreen;
extern int introBackBlue;
extern int introBackAlpha;
extern int introFontRed;
extern int introFontGreen;
extern int introFontBlue;
extern int introFontAlpha;
extern Uint32 introScrollSpeed;
extern Uint32 introPauseTime;

extern int dbStartX;
extern int dbStartY;
extern int dbWidth;
extern int dbHeight;
extern int dbBackRed;
extern int dbBackGreen;
extern int dbBackBlue;
extern int dbBackAlpha;
extern int dbFontRed;
extern int dbFontGreen;
extern int dbFontBlue;
extern int dbFontAlpha;
extern Uint32 dbScrollSpeed;

// Render a scrollbox
bool gui_drawScrollBox(_scrollBox *scrollBox);

// Setup a area with scrolling text
void gui_setupScrollBox(int whichScrollBox, _scrollBox *scrollBox, const string &sourceText);

// Create timer to control the speed of the scrolling intro
void gui_timerScrollSpeedIntro(int newState, Uint32 scrollSpeed);

// Create timer to pause the scrolling text
void gui_timerScrollPauseIntro(int newState, Uint32 pauseTime);

// Get the next line of text up to the scrollBox width
void gui_getNextTextLine(_scrollBox *scrollBox);
