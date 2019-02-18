#version 330

uniform sampler2D   inTexture0;
uniform float       gamma;

in      vec2        texCoord0;
in      vec3        colorKey;
in      vec3        tintColor;

uniform      float       time;

layout(location = 0)    out     vec4        pixelColor;     // Relates to frameBuffer - writing to render buffer 0 - which is linked to the targetTexture as GL_COLOR_ATTACHMENT 0

vec4 textureColor;


void main()
{
	textureColor = texture2D(inTexture0, texCoord0.st);
	if (textureColor.rgb == colorKey.rgb)
		discard;

	if (tintColor.r > -1.0f)
		textureColor.rgb = tintColor;
	//
	// Apply Gamma setting to the texture colors
	vec4 finalGamma = vec4 (1.0f / gamma);
	finalGamma.w = 1.0f;
	pixelColor = pow (textureColor, finalGamma);
}

/*

float random(vec2 p){
    return fract(sin(dot(p,vec2(12.9898 * (time / 100), 78.233))) * 43758.5453 * time);
}

void main(void)
{
vec2 resolution;

resolution.x = 800;
resolution.y = 600;


    vec2 st = (2.0 * gl_FragCoord.xy - resolution) / min(800, 600);
    vec3 c;

   	textureColor = texture2D(inTexture0, texCoord0.st);
   	if (textureColor.rgb == colorKey.rgb)
   		discard;

	if (tintColor.r > -1.0)
		textureColor.rgb = tintColor;

	c.rgb = texture2D(inTexture0, texCoord0.st).rgb;

    // grain
    c += 0.5 * random(st);

    // scanline
    c *= fract(st.y * 40.0 - time * 1.0) < 0.5 ? 0.7 : 1.0;

    pixelColor = vec4(c, 1.0);
}

*/
