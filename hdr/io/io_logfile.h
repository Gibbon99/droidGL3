#pragma once

#include "hdr/system/sys_main.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

#if defined (WIN32)
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

// Log output to file on disk
//void io_logToFile ( const char *format, ... );
void io_logToFile ( std::string format, ... );

// start the log file - if possible
// set fileLoggingOn to true
bool io_startLogFile ( const char *logFileName );

// if the log file is open - close it
void io_closeLogFile();

// Write the passed string to the open logfile
void io_writeToFile ( string textToWrite );

// Process the events put onto the logging queue - run by detached thread
int io_processLoggingEventQueue ( void *ptr );
