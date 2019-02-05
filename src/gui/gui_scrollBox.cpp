#include "hdr/gui/gui_language.h"
#include "hdr/gui/gui_scrollBox.h"
#include "hdr/gui/gui_main.h"

_scrollBox		introScrollBox;
_scrollBox   	databaseScrollBox;


int introStartX;
int introStartY;
int introWidth;
int introHeight;
int introBackRed;
int introBackGreen;
int introBackBlue;
int introBackAlpha;
int introFontRed;
int introFontGreen;
int introFontBlue;
int introFontAlpha;
Uint32 introScrollSpeed;
Uint32 introPauseTime;

int dbStartX;
int dbStartY;
int dbWidth;
int dbHeight;
int dbBackRed;
int dbBackGreen;
int dbBackBlue;
int dbBackAlpha;
int dbFontRed;
int dbFontGreen;
int dbFontBlue;
int dbFontAlpha;
Uint32 dbScrollSpeed;


SDL_TimerID         introScrollTimerID = 0;
SDL_TimerID         introScrollPauseTimerID = 0;
SDL_TimerID         dbScrollTimerID = 0;

bool                doIntroScroll = false;
bool                doDBScroll = false;

int                 newTextLineLength;
int                 currentTextLineLength;
int                 textLineHeight;

//----------------------------------------------------------
//
// Render a scrollbox
bool gui_drawScrollBox(_scrollBox *scrollBox)
//----------------------------------------------------------
{
	int lineCounter = 0;

	for ( const auto &lineIndex : scrollBox->textLine)
	{
		gui_renderText( guiFontName, glm::vec2{scrollBox->startX, scrollBox->startY - scrollBox->scrollY - (scrollBox->fontLineHeight * lineCounter)}, glm::vec3{scrollBox->fontCol.r, scrollBox->fontCol.g, scrollBox->fontCol.b}, guiSurface, lineIndex);
		lineCounter++;
		if (lineCounter == scrollBox->linesToPrint)
			break;
	}

	boxRGBA(renderer,
	    scrollBox->startX + scrollBox->width,
	    (scrollBox->startY - scrollBox->height) + (scrollBox->fontLineHeight - 2),
	    scrollBox->startX,
	    (scrollBox->startY - scrollBox->height) + (scrollBox->fontLineHeight * 2), 0, 0, 0, 255);

	boxRGBA(renderer,
	    scrollBox->startX + scrollBox->width,
	    scrollBox->startY,
	    scrollBox->startX,
	    scrollBox->startY + (scrollBox->fontLineHeight), 0, 0, 0, 255);

	return true;
}

//----------------------------------------------------------
//
// Get the next line of text up to the scrollBox width
void gui_getNextTextLine(_scrollBox *scrollBox)
//----------------------------------------------------------
{
	string    nextWord;
	string    nextLine;

	int previousSpace = 0;
	bool foundLine;
	int i = 0;

	int textLength;
	int boxWidth;

	//
	// Move the text up to the next line
	for (i = scrollBox->numVisibleLinesHeight - 1; i != 0; i--)
	{
		scrollBox->textLine[i] = scrollBox->textLine[i - 1];
	}

	foundLine = false;
	nextLine = "";   // clear it out
	nextWord = "";   // clear it out

	while ( !foundLine )
	{
		nextWord += (scrollBox->sourceText[scrollBox->charPtr]);

		switch (nextWord[i])
		{
			case ' ': // Complete word
				{
					TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, nextLine.c_str(), &currentTextLineLength, &textLineHeight);
					TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, nextWord.c_str(), &newTextLineLength, &textLineHeight);

					textLength = currentTextLineLength + newTextLineLength;

					boxWidth = scrollBox->width;
					if (textLength < boxWidth)
					{
						previousSpace = scrollBox->charPtr;    // remember this space
						nextWord[i+1] = '\0';
						nextLine.append(nextWord);
						nextWord = "";
						i = -1;
					}
					else
					{
						scrollBox->charPtr = previousSpace;
						foundLine = true;
					}
				}
				break;

			case '!':		// line break
				nextWord[i] = '\0';
				nextLine.append(nextWord);
				nextWord = "";
				i = -1;
				foundLine = true;
				scrollBox->charPtr++;
				break;

			case '@':		// line return - space
				nextWord[i] = '\0';
				nextLine.append(nextWord);
				nextWord = "";
				i = -1;
				foundLine = true;
				scrollBox->charPtr++;
				break;

			case '$':		// start a centered text
				nextWord[i] = '\0';
				scrollBox->charPtr++;
				break;

			case '%':		// end a centered text
				nextWord[i] = '\0';
				scrollBox->charPtr++;
				break;

			case '^':		// pause the scrolling text
				nextWord[i] = '\0';
//				scrollBox->scrollDelay = scrollPauseDelayValue;
				scrollBox->charPtr++;
				foundLine = true;
				doIntroScroll = false;
				gui_timerScrollPauseIntro(USER_EVENT_TIMER_ON, introPauseTime);      // Pause in milliseconds
				break;

			default:
				break;
		}

		i++;
		scrollBox->charPtr++;
		if (scrollBox->charPtr > (int)scrollBox->sourceText.size())
		{
			nextLine.append(nextWord);
			foundLine = true;

			scrollBox->charPtr = 0;     // Reset to start of text
		}
	}

	scrollBox->textLine[0] = nextLine;
}


//------------------------------------------------------------------------
//
// Fine scroll the intro scrollbox - called from Timer
Uint32 gui_scrollIntroText(Uint32 interval, void *param )
//------------------------------------------------------------------------
{
	if (!doIntroScroll)
		return interval;

	//
	// Next pixel line
	introScrollBox.scrollY++;

	if (introScrollBox.scrollY > ttfFonts[gui_getFontIndex (guiFontName)].fontHeight)
	{
		introScrollBox.scrollY = 0;
		gui_getNextTextLine(&introScrollBox);
		introScrollBox.linesToPrint++;
		if (introScrollBox.linesToPrint > introScrollBox.numVisibleLinesHeight)
			introScrollBox.linesToPrint = introScrollBox.numVisibleLinesHeight;
	}

	return interval;
}

//------------------------------------------------------------------------
//
// Callback to resume the scrolling text
Uint32 gui_introPauseCallback(Uint32 interval, void *param)
//------------------------------------------------------------------------
{
	doIntroScroll = true;
	gui_timerScrollPauseIntro(USER_EVENT_TIMER_OFF, 0);
	introScrollPauseTimerID = 0;
	return 0;
}


//------------------------------------------------------------------------
//
// Create timer to pause the scrolling text
void gui_timerScrollPauseIntro(int newState, Uint32 pauseTime)
//------------------------------------------------------------------------
{
	switch ( newState )
	{
		case USER_EVENT_TIMER_OFF:
		{
			evt_removeTimer(introScrollPauseTimerID);
			break;
		}
		case USER_EVENT_TIMER_ON:
		{
			if (introScrollPauseTimerID == 0)
				introScrollPauseTimerID = evt_registerTimer(pauseTime, gui_introPauseCallback, "Pause intro scroll");
			break;
		}
		default:
			break;
	}
}

//------------------------------------------------------------------------
//
// Create timer to control the speed of the scrolling intro
void gui_timerScrollSpeedIntro(int newState, Uint32 scrollSpeed)
//------------------------------------------------------------------------
{
	switch ( newState )
	{
		case USER_EVENT_TIMER_OFF:
		{
			doIntroScroll = false;
			break;
		}
		case USER_EVENT_TIMER_ON:
		{
			if (introScrollTimerID == 0)
				introScrollTimerID = evt_registerTimer(scrollSpeed, gui_scrollIntroText, "Intro scroll speed");

			doIntroScroll = true;
			break;
		}
		default:
			break;
	}
}


//----------------------------------------------------------
//
// Setup a area with scrolling text
void gui_setupScrollBox(int whichScrollBox, _scrollBox *scrollBox, const string &sourceText)
//----------------------------------------------------------
{
	switch (whichScrollBox)
	{
		case SCROLLBOX_INTRO:

			scrollBox->startX = introStartX;
			scrollBox->startY = introStartY;

			scrollBox->width =  introWidth;
			scrollBox->height = introHeight;
			scrollBox->backGroundCol = gui_mapRGBA(introBackRed, introBackGreen, introBackBlue, introBackAlpha);
			scrollBox->fontCol = gui_mapRGBA(introFontRed, introFontGreen, introFontBlue, introFontAlpha);
			scrollBox->scrollSpeed = introScrollSpeed;  // double up - remove, use in next line
			gui_timerScrollSpeedIntro(USER_EVENT_TIMER_OFF, scrollBox->scrollSpeed);
			break;

		case SCROLLBOX_DB:
			scrollBox->startX = dbStartX;
			scrollBox->startY = dbStartY;

			scrollBox->width =  dbWidth;
			scrollBox->height = dbHeight;
			scrollBox->backGroundCol = gui_mapRGBA(dbBackRed, dbBackGreen, dbBackBlue, dbBackAlpha);
			scrollBox->fontCol = gui_mapRGBA(dbFontRed, dbFontGreen, dbFontBlue, dbFontAlpha);
			scrollBox->scrollSpeed = dbScrollSpeed;
			break;

		default:
			break;
	}

	scrollBox->scrollY = 0; //scrollBox->height;       // scroll starting position
	scrollBox->scrollDelayCount = 0.0f;           // counter for smooth scrolling
	scrollBox->linesToPrint = 1;                  // how many to display
	scrollBox->charPtr = 0;                       // start of the text

	scrollBox->sourceText = gui_getString(sourceText);			  // String to use - localized version

	scrollBox->numVisibleLinesHeight = scrollBox->height / ttfFonts[gui_getFontIndex (guiFontName)].fontHeight;
	scrollBox->fontLineHeight = ttfFonts[gui_getFontIndex (guiFontName)].fontHeight;

	scrollBox->textLine.reserve( static_cast<unsigned long>(scrollBox->numVisibleLinesHeight));
	//
	// Get memory and clear strings
	for (auto i = 0; i != scrollBox->numVisibleLinesHeight; i++)
		scrollBox->textLine.emplace_back ( "");

//	strcpy(scrollBox->line[8].text, "Accessing help system...");
}
