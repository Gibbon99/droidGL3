#pragma once

#include "hdr/system/sys_main.h"

extern Uint64       startTime;
extern double       deltaTime;
extern bool         g_debugShowHullCircle;
extern bool         g_debugShowHullLines;

extern vec3         testLightPosition;

typedef struct
{
	glm::vec3   position;
	glm::vec3   color;
	GLuint      textureID;
	GLuint      radius;
} _lightCaster;


extern vector<_lightCaster>     lightCasters;

struct _lightHullPoint
{
	double      angle = 0.0;
	glm::vec3   position;
	glm::vec3   texCoord;

	//
	// Run this when inserting a new element to work out sorting
	bool operator< ( const _lightHullPoint &hullPoint ) const
	{
		return (angle < hullPoint.angle);
	}
};

void light_createLightCaster (vec3 lightPosition);

// Add a new light caster for this level
//
// Pass 0 to radius to delete all texture handles and clear vector
void light_addNewCaster(glm::vec3 position, glm::vec3 color, GLuint radius);
