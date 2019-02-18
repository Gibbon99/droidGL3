#version 330

layout (location = 0) in vec3       inPosition;

uniform mat4                        MVP_Matrix;

void main()
{
	gl_Position = MVP_Matrix * vec4(inPosition, 1);
}
