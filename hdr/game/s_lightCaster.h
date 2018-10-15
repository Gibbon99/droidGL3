#pragma once

#include "hdr/system/sys_main.h"

extern vec3    lightPosition;

struct _lightHullPoint
{
	double angle = 0.0;
	glm::vec3 position;

	//
	// Run this when inserting a new element to work out sorting
	bool operator< ( const _lightHullPoint &hullPoint ) const
	{
		return (angle < hullPoint.angle);
	}
};


void do_light();
