#version 330

//
// Draws a texture mapped 2d quad
//

uniform vec2    inScreenSize;

in vec3         inPosition;
in vec2         inTextureCoords;

out vec2        texCoord0;

void main(void)
{
	texCoord0 = inTextureCoords;

	vec2 vertexPosition = inPosition.xy - inScreenSize.xy;
	vertexPosition /= inScreenSize.xy;
	gl_Position =  vec4(vertexPosition,0,1);
}