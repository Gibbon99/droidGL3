#include <hdr/opengl/gl_renderSprite.h>
#include "hdr/io/io_textures.h"
#include "s_hud.h"

bool        drawHUD;

//------------------------------------------------------------------------
//
// Render the HUD graphic and text
void s_renderHUD()
//------------------------------------------------------------------------
{
	glm::vec2           textureSize;
	glm::vec2           scaleBy;
	static auto         haveSize = false;
	static glm::vec2    renderPosition;
	float texCoords[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};

	if (!drawHUD)
		return;

	if (!haveSize)
	{
		textureSize = io_getTextureSize("hud");
		scaleBy = gl_getScaleby("hud");

		textureSize.x *= scaleBy.x;
		textureSize.y *= scaleBy.y;

		renderPosition.x = (winWidth - textureSize.x) / 2;
		renderPosition.y = winHeight - textureSize.y;

		haveSize = true;
	}
//	gl_renderSprite("hud", renderPosition, 1, glm::vec3{0,0,0});
	gl_draw2DQuad (renderPosition, textureSize, "quad3d", io_getTextureID ("hud"), glm::vec3{0, 0, 0}, glm::vec3{-1.0, 0.0, 0.0}, texCoords);
}
