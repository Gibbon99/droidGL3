#version 330

//
// Draws a texture mapped vertex in 3d space
//
// Pass in the color to discard
//

layout (location = 0) in vec3       inPosition;
layout (location = 1) in vec2       inTextureCoords;

uniform	vec3						inColorKey;
uniform vec3                        inTintColor;

out vec2                            texCoord0;
out vec3							colorKey;
out vec3                            tintColor;

uniform mat4                        MVP_Matrix;

void main(void)
{
	texCoord0 = inTextureCoords;
	colorKey = inColorKey;
	tintColor = inTintColor;

	gl_Position = MVP_Matrix * vec4(inPosition, 1.0f);
}