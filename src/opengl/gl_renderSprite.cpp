#include <utility>

#include <hdr/io/io_textures.h>
#include "gl_renderSprite.h"

unordered_map<string, _sprite>      sprites;

//------------------------------------------------------------------------
//
// Create a sprite object
void gl_createSprite(string textureName, glm::vec3 keyColor, int numberOfFrames, glm::vec2 scaleBy)
//------------------------------------------------------------------------
{
	_sprite     tempSprite;

	tempSprite.textureName = textureName;
	tempSprite.numberOfFrames = numberOfFrames;
	tempSprite.scaleBy = scaleBy;
	if (keyColor.r != -1)
	{
		tempSprite.useKeyColor = true;
		tempSprite.keyColor = keyColor;
	}
	else
		tempSprite.useKeyColor = false;

	sprites.insert (std::pair<string, _sprite> (textureName, tempSprite));
}

//---------------------------------------------------------------------
//
// Return the scaling values for a sprite
glm::vec2 gl_getScaleby(string whichSprite)
//---------------------------------------------------------------------
{
	unordered_map<string, _sprite>::const_iterator spriteItr;

	spriteItr = sprites.find (whichSprite);
	if ( spriteItr != sprites.end())
	{
		return spriteItr->second.scaleBy;
	}
	con_print(CON_ERROR, true, "Unable to find sprite [ %s ] to return scaleBy.", whichSprite);
	return glm::vec2{1.0, 1.0}; // Return sane value
}

//---------------------------------------------------------------------
//
// Render a image file
//
// Position is the center of the sprite
void gl_renderSprite(string whichSprite, glm::vec2 position,  glm::vec3 tintColor)
//---------------------------------------------------------------------
{
	glm::vec2   textureSize;

	unordered_map<string, _sprite>::const_iterator spriteItr;

	spriteItr = sprites.find (whichSprite);

	if ( spriteItr != sprites.end ())    // Found
	{
		textureSize = io_getTextureSize(whichSprite);
		textureSize.x *= spriteItr->second.scaleBy.x;
		textureSize.y *= spriteItr->second.scaleBy.y;
//
// Work out size based on numFrames and textureWidth
//
// Get frameNumber from map that is done in think

		if (spriteItr->second.useKeyColor)  // Needs key color shader
		{
			gl_draw2DQuad ( position, textureSize, "colorKey", io_getTextureID (whichSprite), spriteItr->second.keyColor);
		}
		else
		{
			gl_draw2DQuad ( position, textureSize, "quad3d", io_getTextureID (whichSprite), glm::vec3{0,0,0});
		}
	}
	else
	{
		con_print(CON_ERROR, true, "Unable to find sprite [ %s ] to render.");
	}

}
