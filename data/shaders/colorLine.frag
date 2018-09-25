#version 330

in		vec4		outColor;

layout(location = 0)    out vec4        pixelColor;

void main()
{
	pixelColor = outColor;
	//
	// Apply Gamma setting to the output color
//	pixelColor.rgb = pow (pixelColor.rgb, vec3 (1.0 / 0.5f));
//	pixelColor.a = 1.0f;
}
