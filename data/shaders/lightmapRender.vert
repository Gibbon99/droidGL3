#version 330

//
// Draw a lightmap in 3d coords
//

layout (location = 0) in vec3       inPosition;
layout (location = 1) in vec2       inTextureCoords;

out vec2                            texCoord0;

uniform mat4                        MVP_Matrix;

void main(void)
{
	texCoord0 = inTextureCoords;

	gl_Position = MVP_Matrix * vec4(inPosition, 1.0f);
}