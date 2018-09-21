#version 330 core

uniform sampler2D inTexture0;

uniform     float   outerRadius;
uniform     float   gamma;

in          vec2    texCoord0;
in          vec2    outScreenSize;
out         vec4    pixelColor;

void main()
{
	float x = (gl_FragCoord.x - outScreenSize.x / 2.0f) / (outScreenSize.x / 2.0f);
	float y = (gl_FragCoord.y - outScreenSize.y / 2.0f) / (outScreenSize.y / 2.0f);


	vec4 	texColor = texture2D (inTexture0, texCoord0.st);

	float len = sqrt(x * x + y * y);

	if (len > outerRadius)  // discard fragment if outside the circle
//		discard;
		pixelColor.rgb = vec3(1,0,0);
	else
		pixelColor.rgb = pow (texColor.rgb, vec3 (1.0 / gamma));

	pixelColor.a = 1.0f;
};