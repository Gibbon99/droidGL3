#pragma once

extern int		                                currentLanguage;
extern unordered_map<std::string, std::string>  textStrings;

// Return a string based on the key
string gui_getString(string key);

// Add a key and string to the map
void gui_addKeyAndText(std::string key, const std::string &textString);
