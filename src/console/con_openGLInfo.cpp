#include "hdr/system/sys_main.h"

//-----------------------------------------------------------------------------
//
// Display information about the version of OpenGL we are running
int conOpenGLInfo()
//-----------------------------------------------------------------------------
{
	con_print ( CON_TEXT, true, "Vendor: [ %s ]", glGetString ( GL_VENDOR ) );
	con_print ( CON_TEXT, true, "Renderer: [ %s ]", glGetString ( GL_RENDERER ) );
	con_print ( CON_TEXT, true, "Version: [ %s ]", glGetString ( GL_VERSION ) );

	return 1;
};
