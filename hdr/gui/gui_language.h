#pragma once

#include <unordered_map>

extern int		                                currentLanguage;
extern std::unordered_map<std::string, std::string>  textStrings;

// Return a string based on the key
std::string gui_getString(std::string key);

// Add a key and string to the map
void gui_addKeyAndText(std::string key, std::string textString);
