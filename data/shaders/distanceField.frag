#version 330

/* Freetype GL - A C OpenGL Freetype engine
*
* Distributed under the OSI-approved BSD 2-Clause License. See accompanying
* file `LICENSE` for more details.
*/

uniform sampler2D   inTexture0;
in      vec2        texCoord0;
in      vec4        theFontColor;
in      vec4        theFontOutlineColor;
out     vec4        outColor;

const float glyph_center = 0.50;
const float outline_center = 0.55;

vec3 glow_color = vec3(1.0,1.0,1.0);
const float glow_center = 1.25;

void main(void)
{
    vec4 color = texture2D(inTexture0, texCoord0.st);
    float dist = color.r;
    float width = fwidth(dist);
    float alpha = smoothstep(glyph_center-width, glyph_center+width, dist);

    // Smooth
    outColor = vec4(theFontColor.rgb, alpha);

    // Outline
    // float mu = smoothstep(outline_center-width, outline_center+width, dist);
    // vec3 rgb = mix(theFontOutlineColor.rgb, theFontColor.rgb, mu);
    // outColor = vec4(rgb, max(alpha,mu));

    // Glow
    //vec3 rgb = mix(glow_color, glyph_color, alpha);
    //float mu = smoothstep(glyph_center, glow_center, sqrt(dist));
    //gl_FragColor = vec4(rgb, max(alpha,mu));

    // Glow + outline
    //vec3 rgb = mix(glow_color, theFontColor.rgb, alpha);
    //float mu = smoothstep(glyph_center, glow_center, sqrt(dist));

    //color = vec4(rgb, max(alpha,mu));

    //float beta = smoothstep(outline_center-width, outline_center+width, dist);

    //rgb = mix(theFontOutlineColor.rgb, color.rgb, beta);

    //outColor = vec4(rgb, max(color.a,beta));
}
