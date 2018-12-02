#include <hdr/network/net_server.h>
#include <hdr/network/net_client.h>
#include <hdr/game/gam_physicsPlayer.h>
#include "hdr/system/sys_main.h"
#include "hdr/game/gam_physics.h"
#include "hdr/libGL/gl_window.h"
#include "hdr/system/sys_shutdown.h"
#include "hdr/io/io_logfile.h"
#include "hdr/console/con_conScript.h"

//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// \return None
//
// Shutdown gracefully
void sys_shutdownToSystem ()
//-----------------------------------------------------------------------------------------------------
{
	sys_destroyPlayerPhysics ();
	sys_freePhysicsEngine ();
	gl_displayErrors ();
	lib_destroyWindow();
	con_shutDownScriptEngine ();
	io_closeLogFile();
	runThreads = false;
	SDL_Delay (1000);
//	sys_reportMemLeak ("leakReport.txt");
	evt_shutdownMutex ();

	net_shutdownServer ();
	net_shutdownClient ();

	SDL_Quit ();
}
