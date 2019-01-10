#pragma once

#include "hdr/system/sys_main.h"

// Heads towards destination
cpVect sys_getDirection ( cpVect sourcePoint, cpVect destPoint );

// Put four bytes ( chars ) into one int value
int sys_pack4Bytes(char one, char two, char three, char four);

// Returns 4 char bytes into passed in array from INT parameter
void sys_getPackedBytes(int sourceNumber, unsigned char *returnArray);
