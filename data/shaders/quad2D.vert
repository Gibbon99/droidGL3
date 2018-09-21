#version 330

//
// Draws a texture mapped 2d quad
//
layout (location = 0) in vec3         inPosition;
layout (location = 1) in vec2         inTextureCoords;

uniform mat4                          MVP_Matrix;

out vec2                              texCoord0;

void main(void)
{
	texCoord0 = inTextureCoords;

	gl_Position = MVP_Matrix * vec4(inPosition, 1);
}