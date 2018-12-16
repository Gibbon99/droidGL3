/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#version 330

in      vec2 inPosition;
in      vec2 inTextureCoords;
in      vec4 inFontColor;
in      vec4 inFontOutlineColor;
uniform vec2 inScreenSize;

out     vec4 theFontColor;
out     vec4 theFontOutlineColor;
out     vec2 texCoord0;

void main(void)
{
    texCoord0 = inTextureCoords;
    theFontColor = inFontColor;
    theFontOutlineColor = inFontOutlineColor;

    vec2 vertexPosition = inPosition.xy - inScreenSize.xy; // [0..800][0..600] -> [-400..400][-300..300]
    vertexPosition /= inScreenSize.xy;
    gl_Position =  vec4(vertexPosition,0,1);
}
