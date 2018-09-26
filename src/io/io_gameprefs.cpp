#include "hdr/system/sys_main.h"
#include "hdr/io/io_gameprefs.h"

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

	return true;
}
