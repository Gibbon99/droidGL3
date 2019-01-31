#pragma once

#include "hdr/system/sys_main.h"
#include <unordered_map>

typedef struct
{
	string      textureName;
	int         numberOfFrames;
	float       frameWidth;
	float       frameHeight;
	bool        useKeyColor;
	bool        markedForDeletion;
	glm::vec3   keyColor;
	glm::vec3   tintColor;      // Overwrite pixels with this color
	glm::vec2   scaleBy;
	glm::vec2   renderOffset;
	glm::vec2   textureSize;
} _sprite;

extern unordered_map<string, _sprite>       sprites;
extern vector<string>                       droidToSpriteLookup;

// Create the lookup table between droid type and sprite name
void gl_setupDroidToSpriteLookup ();

// Create the sprite objects once textures have finished loading
void gl_createAllSprites ();

// Return the sprite name for a droidType
string gl_getSpriteName ( int droidType );

// Render a image file
//
// Position is the center of the sprite
void gl_renderSprite (string whichSprite, glm::vec2 position, float rotateAngle, int frameNumber, glm::vec3 tintColor);

// Create a sprite object
void gl_createSprite ( string textureName, glm::vec3 keyColor, int numberOfFrames, glm::vec2 scaleBy, glm::vec3 tintColor );

// Return the scaling values for a sprite
glm::vec2 gl_getScaleby(string whichSprite);

// Remove a sprite from the spriteSet
void gl_removeSprite( string textureName );

// Look through the sprite set and delete any that have been marked for deletion from other threads
void gl_cleanSpriteMap ( );

