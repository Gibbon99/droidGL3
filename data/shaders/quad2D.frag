#version 330

uniform sampler2D   inTexture0;

in      vec2        texCoord0;
out     vec4        outColor;

void main()
{
	vec4 texColor = texture2D(inTexture0, texCoord0.st);
	outColor = vec4(texColor.rgb, 1.0f);
}