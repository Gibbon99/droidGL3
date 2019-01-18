#pragma once

// Handle a game event - called by thread
int gam_processGameEventQueue ( void *ptr );

// Handle the events that need to be run in the main loop
// eg: Changes to physics world
int gam_processMainLoopEventQueue();
