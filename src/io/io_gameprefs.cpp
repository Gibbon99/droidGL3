#include <hdr/system/sys_audio.h>
#include "hdr/system/sys_main.h"
#include "hdr/io/io_gameprefs.h"

// TODO: Add way to update ini values from console and then save them to file

//-----------------------------------------------------------------------------------------------------
//
/// \param Pass in the section name, key name and the new value. Will be inserted if it doesn't exist
/// \return True on no errors -  False if a key or value is not found
bool io_setGamePref(const string section, const string keyName, const string keyValue)
//-----------------------------------------------------------------------------------------------------
{
	CSimpleIniA     iniFile;
	SI_Error        returnCode;

	// changing the value of a key
	returnCode = iniFile.SetValue(section.c_str(), keyName.c_str(), keyValue.c_str());
	if (returnCode < 0)
	{
		con_print(CON_ERROR, true, "Unable to update / insert key value [ % ] in section [ %s ]", keyName, section);
		return false;
	}
	//
	// Save the data back to the file
	returnCode = iniFile.SaveFile(GAME_PREFS_FILENAME);
	if (returnCode < 0)
	{
		con_print(CON_ERROR, true, "Failed to save iniFile values to file.");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param Filename to open
/// \return True on no errors -  False if a key or value is not found
bool io_getGamePrefs(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	CSimpleIniA iniFile;
	const char *pszValue;

	iniFile.SetUnicode ();
	iniFile.LoadFile (fileName.c_str());

	// get the value of a key
	pszValue = iniFile.GetValue ("mainSection", "winWidth", "800");
	winWidth = atoi(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "winHeight", "600");
	winHeight = atoi(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "useSound", "1");
	as_useSound = (bool)atoi(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "volSound", "5");
	as_audioVolume = atoi(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "gamma", "0.5");
	g_gamma = (float)atof(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "fullScreen", "0");
	fullScreen = (bool)atoi(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "vsyncType", "1");
	vsyncType = atoi(pszValue);

	pszValue = iniFile.GetValue ("mainSection", "serverName", "127.0.0.1");
	serverName = pszValue;

	pszValue = iniFile.GetValue ("mainSection", "serverPort", "49999");
	serverPort = atoi(pszValue);

	return true;
}
