#version 330

in		vec4		outColor;
//out     vec4        pixelColor;

uniform float   gamma;

out vec4        pixelColor;

void main()
{
	pixelColor = outColor;
	//
	// Apply Gamma setting to the output color
	pixelColor.rgb = pow (pixelColor.rgb, vec3 (1.0 / gamma));
	pixelColor.a = 1.0f;
}
