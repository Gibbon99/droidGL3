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

//-----------------------------------------------------------------------------------------------------
//
/// \param argv None
/// \return None
//
// Process the events put onto the logging queue - run by detached thread
int io_processLoggingEventQueue( void *ptr )
//-----------------------------------------------------------------------------------------------------
{
	_myEventData tempEventData;

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
}

//--------------------------------------------------------
// open the log file for writing
// returns TRUE/FALSE for operation
bool openLogFile ( const char *logFileName )
//--------------------------------------------------------
{
//#define APPEND_LOGFILE 1

#if defined (WIN32)

#ifdef APPEND_LOGFILE	// append logfile entrys
	logFile = _open ( logFileName, _O_WRONLY | _O_APPEND, _S_IREAD | _S_IWRITE );
#else					// or else create a new one each time
	logFile = _creat ( logFileName, _S_IWRITE );
#endif

#else

#ifdef APPEND_LOGFILE	// append logfile entries
	logFile = open ( logFileName, O_WRONLY | O_APPEND, S_IREAD | S_IWRITE );
#else					// or else create a new one each time
	logFile = creat ( logFileName, S_IWRITE );
#endif

#endif

	if ( logFile == -1 )
		{
			// file not found - doesn't exist ??
			// try to create it
#if defined (WIN32)
			logFile = _open ( logFileName, _O_WRONLY | _O_CREAT, _S_IREAD | _S_IWRITE );
#else
			logFile = open ( logFileName, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE );
#endif

			if ( logFile == -1 )	// can't even create it
				return false;	// return failure
			else
				return true;
		}
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
	ssize_t byteswritten;

	if (!fileLoggingOn)
		return;

	#if defined (WIN32)
		if ( ( byteswritten = _write ( logFile, textToWrite, strlen ( textToWrite ) ) ) == -1 )
	#else
		if ((byteswritten = write (logFile, textToWrite.c_str(), strlen (textToWrite.c_str()))) == -1 )
	#endif
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

#if defined (WIN32)
	char		tmpTime[32];
#endif
	//
	// check if filelogging is actually enabled
	//
	if ( !fileLoggingOn )
		return;

	//
	// check and make sure we don't overflow our string buffer
	//
	if ( strlen ( format ) >= MAX_STRING_SIZE - 1 )
		printf ( __FILE__, __LINE__, "String passed to logfile too long", ( MAX_STRING_SIZE - 1 ), strlen ( format ) - ( MAX_STRING_SIZE - 1 ) );

	//
	// get out the passed in parameters
	//
	va_start ( args, format );
	vsprintf ( logText, format, args );
	va_end ( args );
	//
	// get the current time and log to file
	//
#if defined (WIN32)
	_strtime ( tmpTime );
	strcat ( tmpTime, " > " );

	if ( ( byteswritten = _write ( logFile, tmpTime, strlen ( tmpTime ) ) ) == -1 )
		ErrorFatal ( __FILE__, __LINE__, "Write to logfile failed." );

#endif
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
	_strtime ( tmptime );
	_strdate ( tmpdate );
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
