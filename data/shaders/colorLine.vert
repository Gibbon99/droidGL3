#version 330

//
// Draws a colored line with two points
//
layout (location = 0) in vec3       inPosition;

uniform mat4                        MVP_Matrix;
uniform vec4	                    inColor;

out vec4		                    outColor;

void main(void)
{
	outColor = inColor;
/*
	vec2 vertexPosition = inPosition.xy - inScreenSize.xy;
	vertexPosition /= inScreenSize.xy;
	gl_Position =  vec4(vertexPosition,0,1);
	*/
	gl_Position = MVP_Matrix * vec4(inPosition, 1);
}
