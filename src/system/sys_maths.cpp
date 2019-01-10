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
