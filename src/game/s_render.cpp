#include "hdr/game/s_render.h"

vector<_tileTexCoords> tileTexCoords;

//-----------------------------------------------------------------------------
//
// Setup up precalculated coords for the tiles
void gam_calcTileTexCoords(const string textureName)
//-----------------------------------------------------------------------------
{
	int             totalNumTiles;
	int             numTileAcrossInTexture, numTilesDownInTexture;
	vec2            imageSize;
	_tileTexCoords  tempCoords{};   // TODO: Check the affect of this {}

	imageSize = io_getTextureSize(textureName);
	if (imageSize.x == -1)
	{
		con_print(CON_ERROR, true, "Unable to get image size to calculate tile coordinates [ %s ].", textureName.c_str());
		return;
	}

	//
	// How many tiles fit into the texture
	numTileAcrossInTexture = static_cast<int>(imageSize.x / TILE_SIZE);
	numTilesDownInTexture = static_cast<int>(imageSize.y / TILE_SIZE);

	totalNumTiles = numTileAcrossInTexture * numTilesDownInTexture;

	for (int i = 0; i != totalNumTiles; i++)
	{
		tempCoords.texCoord.x = (float) (i % numTileAcrossInTexture) * TILE_SIZE;
		tempCoords.texCoord.y = (float) (i / numTilesDownInTexture) * TILE_SIZE;

		tileTexCoords.push_back (tempCoords);
	}
}
