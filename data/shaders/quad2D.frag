#version 330

uniform sampler2D   inTexture0;
uniform float       gamma;

in      vec2        texCoord0;

out     vec4        pixelColor;     // Relates to frameBuffer - writing to render buffer 0 - which is linked to the targetTexture as GL_COLOR_ATTACHMENT 0

vec4 textureColor;

void main()
{
	textureColor = texture2D(inTexture0, texCoord0.st);
	//
	// Apply Gamma setting to the texture colors

	vec4 finalGamma = vec4 (1.0 / gamma);
	finalGamma.w = 1.0;
	pixelColor = pow (textureColor, finalGamma);

//	pixelColor.rgb = pow (textureColor.rgb, vec3 (1.0 / gamma));
//	pixelColor.a = 1.0f;
}
