#version 330

precision mediump float;

uniform float time;
uniform     vec2 inScreenSize;
in          vec2 inPosition;
out         vec4 fragColor;

uniform vec2 mouse;     // TODO Make a random value

#ifdef GL_ES
precision highp float;
#endif

#extension GL_OES_standard_derivatives : enable

float hash (vec2 p)
{
	return fract(4768.1232345456 * sin(dot (vec2 (12.1235345, 23.54656456), p) ));
}

void main ( void )
{
	vec2 position = (gl_FragCoord.xy / inScreenSize.xy) + mouse / 4.0;

	float color = 0.0;
	color += sin (position.x * cos (time / 15.0) * 80.0) + cos (position.y * cos (time / 15.0) * 10.0);
	color += sin (position.y * sin (time / 10.0) * 40.0) + cos (position.x * sin (time / 25.0) * 40.0);
	color += sin (position.x * sin (time / 5.0) * 10.0) + sin (position.y * sin (time / 35.0) * 80.0);
	color *= sin (time / 10.0) * 0.5;

	fragColor = vec4 (hash (time * vec2 (gl_FragCoord.x, gl_FragCoord.y)) * vec3 (1.0), 1.0);
}