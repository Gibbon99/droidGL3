#version 330

uniform sampler2D   inTexture0;
uniform float       gamma;

in      vec2        texCoord0;
out     vec4        pixelColor;

void main()
{
	vec4 texColor = texture2D(inTexture0, texCoord0.st);
	pixelColor = vec4(texColor.rgb, 1.0f);
	//
	// Apply Gamma setting to the texture colors
	pixelColor.rgb = pow (pixelColor.rgb, vec3 (1.0 / gamma));
	pixelColor.a = 1.0f;
}
