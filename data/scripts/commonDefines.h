

//
// Defines for sound
//
const float AUDIO_PAN_CENTER = 0.0f;
const float AUDIO_PAN_LEFT =  -1.0f;
const float AUDIO_PAN_RIGHT =  1.0f;

const int NET_GAME_SINGLE          = 0;
const int NET_GAME_JOIN_NETWORK    = 1;


const int GUI_COORD_TYPE_PIXEL      = 0;
const int GUI_COORD_TYPE_PERCENT    = 1;

const int GUI_LABEL_CENTER          = 0;
const int GUI_LABEL_LEFT            = 1;
const int GUI_LABEL_RIGHT           = 2;

//
//      Defines for GUI objects
const int GUI_OBJECT_SCREEN         = 1;
const int GUI_OBJECT_BUTTON         = 2;
const int GUI_OBJECT_CHECKBOX       = 3;
const int GUI_OBJECT_TEXTBOX        = 4;
const int GUI_OBJECT_LABEL          = 5;

//
//		Defines for Game Modes

const int MODE_CONSOLE              = 1;
const int MODE_GAME                 = 2;
const int MODE_PAUSE                = 3;
const int MODE_SHUTDOWN             = 4;
const int MODE_LOADING              = 5;
const int MODE_INIT                 = 6;
const int MODE_SPLASH               = 7;
const int MODE_INIT_GAME            = 8;
const int MODE_GUI                  = 9;
const int MODE_INTRO                = 10;
const int MODE_LIFT_VIEW            = 11;
const int MODE_SIDE_VIEW            = 12;
const int MODE_DECK_VIEW            = 13;
const int MODE_DATABASE             = 14;

const int GUI_ACTIVE_COL            = 10;
const int GUI_INACTIVE_COL          = 11;
const int GUI_ACTIVE_CORNER_COL     = 12;
const int GUI_INACTIVE_CORNER_COL   = 13;
const int GUI_ACTIVE_LABEL_COL      = 14;
const int GUI_INACTIVE_LABEL_COL    = 15;

enum sideViewColorsIndex
{
	SIDEVIEW_SHIP_COLOR = 0,
	SIDEVIEW_ACTIVE_DECK_COLOR,
	SIDEVIEW_ENGINE_COLOR,
	SIDEVIEW_LIFT_COLOR,
	SIDEVIEW_ACTIVE_LIFT_COLOR,
	SIDEVIEW_NUM_COLORS,
};

enum mySounds
{
	SND_COLLIDE_1 = 0,
	SND_END_TRANSMISSION_1,
	SND_GREEN_ALERT,
	SND_LIFT_1,
	SND_SCROLLBEEPS,
	SND_TRANSFER_DEADLOCK,
	SND_YELLOW_ALERT,
	SND_CONSOLE_1,
	SND_END_TRANSMISSION_2,
	SND_KEYPRESS_BAD,
	SND_LIFT_2,
	SND_START_1,
	SND_TRANSFER_SELECT,
	SND_CONSOLE_2,
	SND_ENERGY_HEAL,
	SND_KEYPRESS_GOOD,
	SND_LIFT_3,
	SND_START_2,
	SND_TRANSFER_STAGE_1,
	SND_DAMAGE,
	SND_EXPLODE_1,
	SND_LASER,
	SND_LOW_ENERGY,
	SND_START_ALL,
	SND_TRANSFER_STAGE_2,
	SND_DISRUPTOR,
	SND_EXPLODE_2,
	SND_LEVEL_SHUTDOWN,
	SND_RED_ALERT,
	SND_TRANSFER_1,
	SND_TRANSFER_START,
	SND_DOOR,
	NUM_SOUNDS
};

//
// Defines for different languages and strings
//
const int LANG_ENGLISH = 0;
const int LANG_ITALIAN = 1;
