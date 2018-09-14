#include "hdr/console/con_console.h"

//-----------------------------------------------------------------------------
//
// Show the available commands
int conHelp()
//-----------------------------------------------------------------------------
{
	int i;

	for ( i = 0; i != conCurrentNumCommands; i++ )
		{
			{
				if ( conCommands[i].type == CON_COMMAND_SCRIPT )
					{
						con_setColor ( 1.0f, 0.0f, 1.0f, 1.0f );
						con_print ( CON_TEXT, false, "[ %s ] - [ %s ]", conCommands[i].command.c_str(), conCommands[i].usage.c_str() );

					}

				else
					{
						con_setColor ( 1.0f, 1.0f, 0.0f, 1.0f );
						con_print ( CON_TEXT, false, "[ %s ] - [ %s ]", conCommands[i].command.c_str(), conCommands[i].usage.c_str() );
					}
			}
		}

	con_setColor ( 1.0f, 1.0f, 1.0f, 1.0f );

	return 1;
}

//-----------------------------------------------------------------------------
//
// Show the commands added from a script
int showScriptAddedCommands()
//-----------------------------------------------------------------------------
{
	int i;

	for ( i = 0; i != conCurrentNumCommands; i++ )
		{
			if ( conCommands[i].type == CON_COMMAND_SCRIPT )
				con_print ( CON_TEXT, false, "[ %s ] - [ %s ]", conCommands[i].command.c_str(), conCommands[i].usage.c_str() );
		}

	return 1;
}
