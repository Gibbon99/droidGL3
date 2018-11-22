#include "hdr/system/sys_events.h"
#include "hdr/io/io_logfile.h"

//--------------------------------------------------------
//
// Logfile routines
//
// Create a text logfile to track application messages
//
//--------------------------------------------------------

static bool		fileLoggingOn;
static int		logFile;			// file handle to the logfile
FILE			*logFileHandle;

//-----------------------------------------------------------------------------------------------------
//
/// \param argv None
/// \return None
//
// Process the events put onto the logging queue - run by detached thread
int io_processLoggingEventQueue( void *ptr )
//-----------------------------------------------------------------------------------------------------
{
	_myEventData tempEventData = _myEventData ();

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !loggingEventQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (loggingMutex) == 0 )
			{
				tempEventData = loggingEventQueue.front ();
				loggingEventQueue.pop ();
				SDL_UnlockMutex (loggingMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case USER_EVENT_LOGGING_START:

					printf("Start logging file\n");

					io_startLogFile (tempEventData.eventString.c_str());
					break;

				case USER_EVENT_LOGGING_STOP:
					io_closeLogFile ();
					break;

				case USER_EVENT_LOGGING_ADD_LINE:
					io_writeToFile (tempEventData.eventString);
					break;

				default:
					break;
			}
		}
	}
	printf ("LOGGING thread stopped.\n");

	return 0;
}

//--------------------------------------------------------
//
// Open the log file for writing - pass in fileName
// Returns TRUE/FALSE for operation
//
bool openLogFile(const char *logFileName)
//--------------------------------------------------------
{
#if defined (WIN32)
	logFileHandle = fopen(logFileName, "w");
	setbuf(logFileHandle, nullptr);	// Set as unbuffered stream
#endif

#if defined __gnu_linux__
	logFileHandle = fopen(logFileName, "w");
	setbuf(logFileHandle, nullptr);	// Set as unbuffered stream
#endif

	if (logFileHandle == nullptr)
		return false;	// return failure
	else
		return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
//
// Write the passed string to the open logfile
void io_writeToFile(string textToWrite)
//-----------------------------------------------------------------------------------------------------
{
	int bytesWritten;

	if (!fileLoggingOn)
		return;

	#if defined _WIN32
		bytesWritten = fprintf(logFileHandle, "%s", textToWrite.c_str());
	#endif

	#if defined __gnu_linux__
		bytesWritten = fprintf(logFileHandle, "%s", textToWrite.c_str());
	#endif

	if (bytesWritten < 0)
		printf("Write to logfile failed.\n" );
}

//--------------------------------------------------------
// Log output to file on disk
// TODO IS this thread safe now it's passing events
void io_logToFile ( const char *format, ... )
//--------------------------------------------------------
{
	va_list		args;
	char		logText[MAX_STRING_SIZE];

	//
	// check and make sure we don't overflow our string buffer
	//
	if ( strlen (format) >= MAX_STRING_SIZE - 1 )
		printf ("String passed to logfile too long max [ %i ] - [ %i ]", (MAX_STRING_SIZE - 1), (int) strlen (format) - (MAX_STRING_SIZE - 1));

	//
	// get out the passed in parameters
	//
	va_start (args, format);
	vsprintf (logText, format, args);
	va_end (args);

	//
	// check if file logging is actually enabled
	//
	if ( !fileLoggingOn )
	{
		printf("%s\n", logText);
		return;
	}
	//
	// put a linefeed onto the end of the text line
	// and send it to the logging queue
	strcat ( logText, "\n" );

	evt_sendEvent (USER_EVENT_LOGGING, USER_EVENT_LOGGING_ADD_LINE, 0, 0, 0, vec2 (), vec2 (), logText);
}

//--------------------------------------------------------
// log the current time and date to the log file
void logTimeToFile()
//--------------------------------------------------------
{
	char tmptime[128];
	char tmpdate[128];

#if defined (WIN32)
	_strtime_s ( tmptime );
	_strdate_s ( tmpdate );
#else
	strcpy ( tmptime, "Unknown" );
	strcpy ( tmpdate, "Unknown" );
#endif

	io_logToFile ( "%s\t%s", tmpdate, tmptime );
}

//--------------------------------------------------------
// start the log file - if possible
// set fileLoggingOn to true
bool io_startLogFile ( const char *logFileName )
//--------------------------------------------------------
{
	fileLoggingOn = false;

	if ( !openLogFile ( logFileName ) )
		{
			printf("Couldn't create the logfile - check file permissions or disk space. Exiting.\n" );
			fileLoggingOn = false;
			return false;
		}
	else
		{
			fileLoggingOn = true;
			io_logToFile ( "-----------------------------------------------------------------------------" );
			io_logToFile ( "Log file opened:" );
			logTimeToFile();
			io_logToFile ( "Logfile started..." );
			return true;
		}
}

//--------------------------------------------------------
// if the log file is open - close it
void io_closeLogFile()
//--------------------------------------------------------
{
	if ( fileLoggingOn )
		{
			io_logToFile ( "Log file closed:" );
			logTimeToFile();
			io_logToFile ( "-----------------------------------------------------------------------------\n\n" );
#if defined (WIN32)
			_close ( logFile );
#else
			close ( logFile );
#endif
			fileLoggingOn = false;
		}
}
