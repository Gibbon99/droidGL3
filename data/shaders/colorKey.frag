#version 330

uniform sampler2D   inTexture0;
uniform float       gamma;

in      vec2        texCoord0;
in      vec3        colorKey;

layout(location = 0)    out     vec4        pixelColor;     // Relates to frameBuffer - writing to render buffer 0 - which is linked to the targetTexture as GL_COLOR_ATTACHMENT 0

vec4 textureColor;

void main()
{
	textureColor = texture2D(inTexture0, texCoord0.st);
	if (textureColor.rgb == colorKey.rgb)
		discard;
	//
	// Apply Gamma setting to the texture colors
	vec4 finalGamma = vec4 (1.0 / gamma);
	finalGamma.w = 1.0;
	pixelColor = pow (textureColor, finalGamma);
}
