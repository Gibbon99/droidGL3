#pragma once

#include "hdr/system/sys_main.h"

// Start the transfer process by starting the first event timer
void gam_initTransfer(uint whichDroid);

// Handle an transfer game user event - called from Transfer Thread
int gam_processTransferEventQueue ( void *ptr );

// Check for input or sound ending for Transfer
void gam_processTransfer();

// Handle an input event for while in transfer mode
void gam_transferHandleInputEvent (int eventAction, int eventType, int eventSource);