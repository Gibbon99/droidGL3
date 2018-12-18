#include "hdr/system/sys_main.h"
#include "hdr/gui/gui_language.h"
#include <unordered_map>

int		currentLanguage;

unordered_map<std::string, std::string> textStrings;

//--------------------------------------------------------
//
// Return a string based on the key
string gui_getString(string key)
//--------------------------------------------------------
{
	std::unordered_map<std::string, std::string>::const_iterator stringIndex;

	stringIndex = textStrings.find(key);

	if (stringIndex == textStrings.end())
	{
		return key + " not found.";
	}

	return stringIndex->second;
}

//--------------------------------------------------------
//
// Add a key and string to the map
void gui_addKeyAndText(std::string key, const std::string &textString)
//--------------------------------------------------------
{
	textStrings.insert ( {key, textString} );
}
