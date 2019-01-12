#include "hdr/system/sys_maths.h"

//-----------------------------------------------------------------------------
//
// Heads towards destination
cpVect sys_getDirection ( cpVect sourcePoint, cpVect destPoint )
//-----------------------------------------------------------------------------
{
	cpVect tempDirection;

	tempDirection = cpvsub (sourcePoint, destPoint);
	tempDirection = cpvnormalize (tempDirection);

	return tempDirection;
}

//-----------------------------------------------------------------------------
//
// Put four bytes ( chars ) into one int value
int sys_pack4Bytes(char one, char two, char three, char four)
//-----------------------------------------------------------------------------
{
	int returnValue = 0;

	returnValue <<= 8;
	returnValue |= one;
	returnValue <<= 8;
	returnValue |= two;
	returnValue <<= 8;
	returnValue |= three;
	returnValue <<= 8;
	returnValue |= four;

	return returnValue;
}

//-----------------------------------------------------------------------------
//
// Returns 4 char bytes into passed in array from INT parameter
void sys_getPackedBytes(int sourceNumber, unsigned char *returnArray)
//-----------------------------------------------------------------------------
{
	returnArray[0] = static_cast<unsigned char>((sourceNumber >> 24) & 0xff);
	returnArray[1] = static_cast<unsigned char>((sourceNumber >> 16) & 0xff);
	returnArray[2] = static_cast<unsigned char>((sourceNumber >> 8) & 0xff);
	returnArray[3] = static_cast<unsigned char>(sourceNumber & 0xff);
}

//----------------------------------------------------------------------
//
// Is an object visible on the screen
bool sys_visibleOnScreen ( cpVect worldCoord, int shapeSize )
//----------------------------------------------------------------------
{
	SDL_Rect        screenArea;

	if ( ( worldCoord.x < 0 ) || ( worldCoord.y < 0 ) )
		return false;

	screenArea.x = static_cast<int>(viewableScreenCoord.x - (shapeSize * 2 ));
	screenArea.y = static_cast<int>(viewableScreenCoord.y - (shapeSize * 2 ));
	screenArea.w= winWidth + shapeSize;
	screenArea.h= winHeight + shapeSize;

	if ( worldCoord.x < screenArea.x )
		return false;

	if ( worldCoord.y < screenArea.y )
		return false;

	if ( worldCoord.x > screenArea.x + ( screenArea.w + ( shapeSize * 2 ) ) )
		return false;

	if ( worldCoord.y > screenArea.y + ( screenArea.h + ( shapeSize * 2 ) ) )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
//
// Convert worldPosition coords to screen coords
cpVect sys_worldToScreen ( cpVect worldPos, int shapeSize )
//-----------------------------------------------------------------------------
{
	cpVect  screenCoords;

	if ( true == sys_visibleOnScreen ( worldPos, shapeSize ) )
	{
		screenCoords.x = worldPos.x - viewableScreenCoord.x;
		screenCoords.y = worldPos.y - viewableScreenCoord.y;
	}
	else
	{
		screenCoords.x = -1;
		screenCoords.y = -1;

		screenCoords.x = worldPos.x - viewableScreenCoord.x;
		screenCoords.y = worldPos.y - viewableScreenCoord.y;
	}
	return screenCoords;
}
