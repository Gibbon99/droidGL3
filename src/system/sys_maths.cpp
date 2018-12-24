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
