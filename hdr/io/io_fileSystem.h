#pragma once

#include "hdr/system/sys_main.h"

// Get the length of a file
int io_getFileSize ( const char *fileName );

// Start the packfile system
bool io_startFileSystem();

// Load a text file into a pointer
int io_getFileIntoMemory ( const char *fileName, void *results );

// Check if a file exists
bool io_doesFileExist(string fileName);
