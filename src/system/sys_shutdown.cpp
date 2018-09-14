#include "hdr/system/sys_main.h"

#include "hdr/libGL/gl_window.h"
#include "hdr/system/sys_shutdown.h"
#include "hdr/io/io_logfile.h"

//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// \return None
//
// Shutdown gracefully
void sys_shutdownToSystem ()
//-----------------------------------------------------------------------------------------------------
{
	lib_destroyWindow();
	io_closeLogFile();
	runThreads = false;
	SDL_Delay (1000);
	evt_shutdownMutex ();
	SDL_Quit ();
}