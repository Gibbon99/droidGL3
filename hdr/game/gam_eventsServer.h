#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/system/sys_events.h"

// Process events coming into the server
int gam_processServerEventQueue ( void *ptr );