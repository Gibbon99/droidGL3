#version 330

//
// Draws a colored disc
//

uniform vec2    inScreenSize;

in vec3         inPosition;
in vec2         inTextureCoords;

out vec4        outPosition;
out vec2        texCoord0;
out vec2        outScreenSize;

void main(void)
{
	texCoord0 = inTextureCoords;
	outScreenSize = inScreenSize;

	vec2 vertexPosition = inPosition.xy - inScreenSize.xy;
	vertexPosition /= inScreenSize.xy;

	gl_Position =  vec4(vertexPosition,0,1);
}
