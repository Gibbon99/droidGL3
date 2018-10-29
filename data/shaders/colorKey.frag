#version 330

uniform sampler2D   inTexture0;
uniform float       gamma;

in      vec2        texCoord0;
in      vec3        colorKey;
in      vec3        tintColor;

layout(location = 0)    out     vec4        pixelColor;     // Relates to frameBuffer - writing to render buffer 0 - which is linked to the targetTexture as GL_COLOR_ATTACHMENT 0

vec4 textureColor;

void main()
{
	textureColor = texture2D(inTexture0, texCoord0.st);
	if (textureColor.rgb == colorKey.rgb)
		discard;

	if (tintColor.r > -1.0)
		textureColor.rgb = tintColor;
	//
	// Apply Gamma setting to the texture colors
	vec4 finalGamma = vec4 (1.0 / gamma);
	finalGamma.w = 1.0;
	pixelColor = pow (textureColor, finalGamma);
}
