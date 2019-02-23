#include "hdr/system/sys_main.h"

int thinkFpsPrint, fpsPrint, fps, thinkFPS;

SDL_TimerID fpsTimerID;

//-----------------------------------------------------------------------------
//
// Callback to get the FPS
static Uint32 SDLCALL fpsTimerCallback(Uint32 interval, void *param )
//-----------------------------------------------------------------------------
{
	fpsPrint = fps;
	fps = 0;
	thinkFpsPrint = thinkFPS;
	thinkFPS = 0;
	return interval;
}

//-----------------------------------------------------------------------------
//
// Init timing variables and start Timer to get current FPS per second
void sys_initTimingVars ()
//-----------------------------------------------------------------------------
{
	fps = 0;
	thinkFPS = 0;

	fpsTimerID = evt_registerTimer (1000, fpsTimerCallback, "FPS Counter", 0);
}
