#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/physfs/physfs.h"

// Get the length of a file
PHYSFS_sint64 io_getFileSize ( const char *fileName );

// Start the packfile system
bool io_startFileSystem();

// Load a text file into a pointer
int io_getFileIntoMemory ( const char *fileName, void *results );

// Return the state of the file system
bool io_isFileSystemReady();

// Check if a file exists
bool io_doesFileExist(string fileName);
