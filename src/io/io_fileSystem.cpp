#include "hdr/system/sys_main.h"
#include "hdr/io/io_logfile.h"
#include "hdr/physfs/physfs.h"

bool        fileSystemReady = false;

//--------------------------------------------------------
//
// Return the state of the file system
bool io_isFileSystemReady()
//--------------------------------------------------------
{
	return fileSystemReady;
}

//--------------------------------------------------------
//
// Get archive types supported by this version
bool io_getArchivers ( void )
//--------------------------------------------------------
{
	const PHYSFS_ArchiveInfo **rc = PHYSFS_supportedArchiveTypes();
	const PHYSFS_ArchiveInfo **i;

	io_logToFile ( "Packfile: Supported archive types." );

	if ( *rc == NULL )
		{
			io_logToFile ( "Packfile: Error: No archive types found." );
			return false;

		}

	else
		{
			for ( i = rc; *i != NULL; i++ )
				{
					io_logToFile ( "Packfile: [ %s ] : [ %s ]", ( *i )->extension, ( *i )->description );
				}
		}

	return true;
}

// ---------------------------------------------------------------------------
//
// Start the packfile system
bool io_startFileSystem()
// ---------------------------------------------------------------------------
{
	PHYSFS_Version compiled;
	PHYSFS_Version linked;

	if ( PHYSFS_init ( NULL ) == 0 )
		{
			con_print(CON_ERROR, true, "Error: Filesystem failed to start - [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}

	PHYSFS_VERSION ( &compiled );
	PHYSFS_getLinkedVersion ( &linked );

	con_print (CON_INFO, true, "Compiled against PhysFS version %d.%d.%d.", compiled.major, compiled.minor, compiled.patch );
	con_print (CON_INFO, true, "Linked against PhysFS version %d.%d.%d.",  linked.major, linked.minor, linked.patch );

	//
	// Setup directory to write if needed
	if ( 0 == PHYSFS_setWriteDir ( "data" ) )
		{
			con_print (CON_ERROR, true, "Failed to set write path [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			con_print (CON_ERROR, true, "The directory [ data ] holding all the data files is not present. Check the installation." );
			printf ( "ERROR: The directory [ data ] holding all the data files is not present. Check the installation.\n" );
			fileSystemReady = false;
			return false;
		}
	//
	// Set base directory
	if ( 0 == PHYSFS_mount ( "data", "/", 1 ) )
		{
			con_print (CON_ERROR, true, "Failed to set search path - data [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}
	//
	// Add directory for loading scripts - move to archive file
	if ( 0 == PHYSFS_mount ( "data//scripts","/", 1 ) )
		{
			con_print (CON_ERROR, true, "Failed to set search path - scripts [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}
	//
	// Add directory for loading shaders - move to archive file
	if ( 0 == PHYSFS_mount ( "data//shaders", "/", 1 ) )
		{
			con_print (CON_ERROR, true, "Failed to set search path - shaders [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}
	//
	// Add directory for loading textures - move to archive file
	if ( 0 == PHYSFS_mount ( "data//textures", "/", 1 ) )
		{
			con_print (CON_ERROR, true, "Failed to set search path - textures [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}
	//
	// Add directory for loading old levels for tile information
	if ( 0 == PHYSFS_mount ( "data//maps", "/", 1 ) )
		{
			con_print (CON_ERROR, true, "Failed to set search path - maps [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}
	//
	// Add archive file
	if ( 0 == PHYSFS_mount ( "data//data.zip", "/", 1 ) )
		{
			con_print (CON_ERROR, true, "Failed to set search path - data.zip - [ %s ]", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ) );
			fileSystemReady = false;
			return false;
		}
	//
	// What compression types are available
	io_getArchivers();

	fileSystemReady = true;

	return true;
}


//-----------------------------------------------------------------------------
//
// Get the length of a file
PHYSFS_sint64 io_getFileSize ( const char *fileName )
//-----------------------------------------------------------------------------
{
	PHYSFS_file		*compFile;
	PHYSFS_sint64	fileLength;

	if ( false == fileSystemReady )
		{
			con_print(CON_ERROR, true, "PHYSFS system has not been initialised. Can't process [ %s ].", fileName );
			return -1;
		}

	//
	// Get a handle to the file
	compFile = PHYSFS_openRead ( fileName );

	if ( NULL == compFile )
		{
			con_print(CON_ERROR, true, "Filesystem can't open file [ %s ] - [ %s ].", fileName, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return -1;
		}

	//
	// Get it's size
	fileLength = PHYSFS_fileLength ( compFile );

	if ( -1 == fileLength )
		{
			con_print(CON_ERROR, true, "Unable to determine file length for [ %s ] - [ %s ].", fileName, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return -1;
		}

	//
	// Finished with it now so close it
	PHYSFS_close ( compFile );

	return fileLength;
}
// ---------------------------------------------------------------------------
//
// Load a file into a pointer
int io_getFileIntoMemory ( const char *fileName, void *results )
// ---------------------------------------------------------------------------
{
	PHYSFS_file		*compFile = nullptr;
	PHYSFS_sint64	fileLength;

	if ( !fileSystemReady )
		{
			con_print(CON_ERROR, true, "PHYSFS system has not been initialised. Can't load [ %s ].", fileName );
			return -1;
		}

	//
	// Get a handle to the file
	compFile = PHYSFS_openRead ( fileName );

	if ( nullptr == compFile )
		{
			con_print(CON_ERROR, true, "Filesystem can't open file [ %s ] - [ %s ].", fileName, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return -1;
		}

	//
	// Get it's size
	fileLength = PHYSFS_fileLength ( compFile );

	if ( -1 == fileLength )
		{
			con_print(CON_ERROR, true, "Unable to determine file length for [ %s ] - [ %s ].", fileName, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			PHYSFS_close (compFile);
			return -1;
		}

	//
	// Read contents of file into the pointer
	PHYSFS_sint64 returnCode = PHYSFS_readBytes ( compFile, ( void * ) results, fileLength);

	if ( -1 == returnCode )
		{
			con_print(CON_ERROR, true, "Filesystem read failed - [ %s ] for [ %s ].", PHYSFS_getErrorByCode ( PHYSFS_getLastErrorCode() ), fileName );
			PHYSFS_close (compFile);
			return -1;
		}

	//
	// Finished with it now so close it
	PHYSFS_close ( compFile );

	return 1;
}

// ---------------------------------------------------------------------------
//
// Check if a file exists
bool io_doesFileExist(string fileName)
// ---------------------------------------------------------------------------
{
	if ( !fileSystemReady )
	{
		con_print(CON_ERROR, true, "File system not ready. Can not check for file [ %s ]", fileName.c_str());
		return false;
	}
	
	return static_cast<bool>(PHYSFS_exists( fileName.c_str()));
}
