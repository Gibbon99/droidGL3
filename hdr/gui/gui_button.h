#pragma once

extern int              focusAnimateIntervalValue;      // Set from script

// Create a SDL_Surface and draw to it
void gui_drawButton(int objectIndex, bool hasFocus);

// Use a timer to animate the colors for the active ( has focus ) button
void gui_timerFocusAnimation(bool runState);
