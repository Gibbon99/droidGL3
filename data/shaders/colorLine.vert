#version 330

//
// Draws a colored line with two points
//
uniform vec2    inScreenSize;
uniform vec4	inColor;
in vec3         inPosition;

out vec4		outColor;

void main(void)
{
	outColor = inColor;

	vec2 vertexPosition = inPosition.xy - inScreenSize.xy;
	vertexPosition /= inScreenSize.xy;
	gl_Position =  vec4(vertexPosition,0,1);
}
