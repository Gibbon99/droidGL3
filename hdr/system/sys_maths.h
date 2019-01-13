#pragma once

#include "hdr/system/sys_main.h"

// Heads towards destination
cpVect sys_getDirection ( cpVect sourcePoint, cpVect destPoint );

// Put four bytes ( chars ) into one int value
int sys_pack4Bytes(char one, char two, char three, char four);

// Returns 4 char bytes into passed in array from INT parameter
void sys_getPackedBytes(int sourceNumber, unsigned char *returnArray);

// Is an object visible on the screen
bool sys_visibleOnScreen ( cpVect worldCoord, int shapeSize );

// Convert worldPosition coords to screen coords
cpVect sys_worldToScreen ( cpVect worldPos, int shapeSize );

// Get the direction of a vector
cpVect sys_getVectorDirection(cpVect vectA, cpVect vectB);

// Get the distance between two points
int sys_getDistance(cpVect point1, cpVect point2);
