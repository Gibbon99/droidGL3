#pragma once

#include "hdr/gui/gui_main.h"
#include "hdr/gui/gui_checkBox.h"


// Draw a Checkbox
// Start is top left of box - width and height is just size of box
// Text length is not part of the overall size
void gui_drawCheckbox (int objectIndex, bool hasFocus );

// Called from script to update the state of the checked status
void gui_updateCheckedStatus(std::string whichCheckBox, bool newState);
