#include <utility>

#include <hdr/io/io_textures.h>
#include "gl_renderSprite.h"

unordered_map<string, _sprite>      sprites;
vector<string>      droidToSpriteLookup;

//------------------------------------------------------------------------
//
// Create the sprite objects once textures have finished loading
void gl_createAllSprites()
//------------------------------------------------------------------------
{
	for ( const auto &indexItr : droidToSpriteLookup )
	{
		gl_createSprite (indexItr, glm::vec3{1.0f, 0.0f, 1.0f}, 9, glm::vec2{1.0f, 1.0f}, glm::vec3{0.0, 0.0, 0.0f});
	}

	sprites.at("001").keyColor = glm::vec3{0, 0, 0};
	sprites.at("001").tintColor = glm::vec3{1.0, 1.0, 1.0};        // Keep 001 white to start with
	sprites.at("001").scaleBy = glm::vec2{1.0f, 1.0f};

	gl_createSprite ("hud", glm::vec3{0.0f, 0.0f, 0.0f}, 1, glm::vec2{1.3f, 1.2f}, glm::vec3{-1.0, 0.0, 0.0});
	gl_createSprite ("splash", glm::vec3{-1.0f, 0.0f, 0.0f}, 1, glm::vec2{1.6f, 1.2f}, glm::vec3{-1.0, 0.0, 0.0});
}

//------------------------------------------------------------------------
//
// Create the lookup table between droid type and sprite name
void gl_setupDroidToSpriteLookup()
//------------------------------------------------------------------------
{
	droidToSpriteLookup.reserve(24);

	droidToSpriteLookup.emplace_back ("001");
	droidToSpriteLookup.emplace_back ("123");
	droidToSpriteLookup.emplace_back ("139");
	droidToSpriteLookup.emplace_back ("247");
	droidToSpriteLookup.emplace_back ("249");
	droidToSpriteLookup.emplace_back ("296");
	droidToSpriteLookup.emplace_back ("302");
	droidToSpriteLookup.emplace_back ("329");
	droidToSpriteLookup.emplace_back ("420");
	droidToSpriteLookup.emplace_back ("476");
	droidToSpriteLookup.emplace_back ("493");
	droidToSpriteLookup.emplace_back ("516");
	droidToSpriteLookup.emplace_back ("571");
	droidToSpriteLookup.emplace_back ("598");
	droidToSpriteLookup.emplace_back ("614");
	droidToSpriteLookup.emplace_back ("615");
	droidToSpriteLookup.emplace_back ("629");
	droidToSpriteLookup.emplace_back ("711");
	droidToSpriteLookup.emplace_back ("742");
	droidToSpriteLookup.emplace_back ("751");
	droidToSpriteLookup.emplace_back ("821");
	droidToSpriteLookup.emplace_back ("834");
	droidToSpriteLookup.emplace_back ("883");
	droidToSpriteLookup.emplace_back ("999");

	//
	// Load each texture
	for ( const auto &indexItr : droidToSpriteLookup)
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_LOAD, 0, 0, 0, vec2 (), vec2 (), indexItr + ".bmp");
	}
}

//------------------------------------------------------------------------
//
// Return the sprite name for a droidType
string gl_getSpriteName(int droidType)
//------------------------------------------------------------------------
{
	if ((droidType < 0) || (droidType > droidToSpriteLookup.size()))
		return "ERROR - Invalid droidType";

	return droidToSpriteLookup[droidType];
}

//------------------------------------------------------------------------
//
// Create a sprite object
void gl_createSprite(string textureName, glm::vec3 keyColor, int numberOfFrames, glm::vec2 scaleBy, glm::vec3 tintColor)
//------------------------------------------------------------------------
{
	static int  errorCount = 0;
	_sprite     tempSprite;

	tempSprite.textureName = textureName;
	tempSprite.numberOfFrames = numberOfFrames;
	tempSprite.scaleBy = scaleBy;
	tempSprite.tintColor = tintColor;
	if (keyColor.r != -1)
	{
		tempSprite.useKeyColor = true;
		tempSprite.keyColor = keyColor;
	}
	else
		tempSprite.useKeyColor = false;

	tempSprite.textureSize = io_getTextureSize (textureName);
	if ( tempSprite.textureSize.x < 0)
	{
		if (errorCount == 0)
		{
			errorCount++;
			con_print (CON_ERROR, true, "Attempting to access textureSize for missing texture [ %s ]", textureName.c_str ());
		}
		else
			errorCount++;

		return;
	}
	tempSprite.frameWidth = (tempSprite.textureSize.x / tempSprite.numberOfFrames ) / tempSprite.textureSize.x;
	tempSprite.frameHeight = tempSprite.textureSize.y;

	tempSprite.renderOffset.x = (tempSprite.textureSize.x / tempSprite.numberOfFrames) * 0.5f;
	tempSprite.renderOffset.y = tempSprite.textureSize.y * 0.5f;

	sprites.insert (std::pair<string, _sprite> (textureName, tempSprite));
}

//---------------------------------------------------------------------
//
// Return the scaling values for a sprite
glm::vec2 gl_getScaleby(string whichSprite)
//---------------------------------------------------------------------
{
	static int errorCount = 0;

	unordered_map<string, _sprite>::const_iterator spriteItr;

	spriteItr = sprites.find (whichSprite);
	if ( spriteItr != sprites.end())
	{
		return spriteItr->second.scaleBy;
	}

	if (errorCount == 0)
	{
		errorCount++;
		con_print (CON_ERROR, true, "Unable to find sprite [ %s ] to return scaleBy.", whichSprite);
	}
	else
		errorCount++;

	return glm::vec2{1.0, 1.0}; // Return sane value
}

//---------------------------------------------------------------------
//
// Render a image file
//
// Position is the center of the sprite
void gl_renderSprite(string whichSprite, glm::vec2 position, int frameNumber, glm::vec3 tintColor)
//---------------------------------------------------------------------
{
	static int          errorCount = 0;
	glm::vec2           textureSize;
	float               texCoords[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
	float               frameWidth;
	glm::vec2           framePosition;
	glm::vec2           renderPosition;
    unordered_map<string, _sprite>::const_iterator spriteItr;

	spriteItr = sprites.find (whichSprite);

	if ( spriteItr != sprites.end ())    // Found
	{
		textureSize = spriteItr->second.textureSize;

		textureSize.x = spriteItr->second.textureSize.x / spriteItr->second.numberOfFrames;

		textureSize.x *= spriteItr->second.scaleBy.x;
		textureSize.y *= spriteItr->second.scaleBy.y;



		frameWidth = spriteItr->second.frameWidth;
		framePosition.x = frameWidth * frameNumber;
		framePosition.y = 0.0f;

		texCoords[0] = framePosition.x;
		texCoords[1] = 1.0f;

		texCoords[2] = framePosition.x;
		texCoords[3] = framePosition.y;

		texCoords[4] = framePosition.x + frameWidth;
		texCoords[5] = framePosition.y;

		texCoords[6] = framePosition.x + frameWidth;
		texCoords[7] = 1.0f;

		renderPosition = position;

		renderPosition.x -= spriteItr->second.renderOffset.x;
		renderPosition.y -= spriteItr->second.renderOffset.y;

		if (spriteItr->second.useKeyColor)  // Needs key color shader
		{
			gl_draw2DQuad ( renderPosition, textureSize, "colorKey", io_getTextureID (whichSprite), spriteItr->second.keyColor, spriteItr->second.tintColor, texCoords);
		}
		else
		{
			gl_draw2DQuad ( position, textureSize, "quad3d", io_getTextureID (whichSprite), glm::vec3{0,0,0}, spriteItr->second.tintColor, texCoords);
		}
	}
	else    // Sprite name could not be found
	{
		if (errorCount == 0)
		{
			errorCount++;
			con_print (CON_ERROR, true, "Unable to find sprite [ %s ] to render.");
		}
		else
			errorCount++;
	}
}
