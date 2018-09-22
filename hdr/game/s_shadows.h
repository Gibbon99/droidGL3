#pragma once

#include "hdr/system/sys_main.h"
#include <set>

struct _shadowHullPoint
{
	double angle = 0.0;
	glm::vec3 position;

	//
	// Run this when inserting a new element to work out sorting
	bool operator< ( const _shadowHullPoint &hullPoint ) const
	{
		return (angle < hullPoint.angle);
	}
};

extern std::set <_shadowHullPoint> shadowHull;

void gam_drawAllObjects ( float interpolate );

void gam_findIntersetPoint ();

void gam_50Rays ();

void gam_doAngleRays ();

//char get_lineIntersection ( _lineSegment ray, _lineSegment segment, float *i_x, float *i_y );