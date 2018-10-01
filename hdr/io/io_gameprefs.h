#pragma once

#include "hdr/io/simpleIni/io_simpleIni.h"

bool io_getGamePrefs ( std::string fileName );

bool io_setGamePref ( std::string section, std::string keyName, std::string keyValue );
