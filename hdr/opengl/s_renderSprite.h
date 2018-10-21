#pragma once

#include "hdr/system/sys_main.h"
#include <unordered_map>

typedef struct
{
	string      textureName;
	int         currentFrame;
	int         numberOfFrames;
	bool        useKeyColor;
	glm::vec3   keyColor;
	glm::vec2   scaleBy;
} _sprite;

extern unordered_map<string, _sprite>      sprites;

// Render a image file
//
// Position is the center of the sprite
void gl_renderSprite(string whichSprite, glm::vec2 position,  glm::vec3 tintColor);

// Create a sprite object
void gl_createSprite(string textureName, glm::vec3 keyColor, int numberOfFrames, glm::vec2 scaleBy);

// Return the scaling values for a sprite
glm::vec2 gl_getScaleby(string whichSprite);