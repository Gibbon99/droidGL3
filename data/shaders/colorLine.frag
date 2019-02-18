#version 330

uniform float       gamma;
in		vec4		outColor;

layout(location = 0)    out vec4        pixelColor;

void main()
{
	pixelColor = outColor;

    //
    // Apply Gamma setting to the texture colors
//    vec4 finalGamma = vec4 (1.0 / gamma);
  //  finalGamma.w = 1.0;
    //pixelColor = pow (pixelColor, finalGamma);
}
