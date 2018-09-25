#include <hdr/io/io_textures.h>
#include <hdr/opengl/gl_openGLWrap.h>
#include <hdr/io/io_mouse.h>
#include "hdr/game/s_shadows.h"
#include "hdr/opengl/gl_shaders.h"

#include <set>

static bool objectsAdded = false;

typedef struct {
	glm::vec3 position;
	glm::vec2 textureCoords;
} _tile;

typedef struct {
	glm::vec3       start;
	glm::vec3       end;
} _lineSegment;

vector<_tile>       tiles;

vector<_lineSegment>    lineSegments;

vector<glm::vec3>       uniquePoints;
vector<double>          uniqueAngles;

set<_shadowHullPoint>   shadowHull;
vector<glm::vec3>       circlePoints;

void DrawCircle ( float cx, float cy, float r, int num_segments )
{
	float theta = 2 * 3.1415926 / float (num_segments);
	float c = cosf (theta);//precalculate the sine and cosine
	float s = sinf (theta);
	float t;

	float x = r;//we start at angle = 0
	float y = 0;

	for ( int ii = 0; ii < num_segments; ii++ )
	{
		circlePoints.push_back(glm::vec3(x + cx, y+cy, 0.0f));
		uniquePoints.push_back (glm::vec3 (x + cx, y + cy, 0.0f));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}

	bool startPoint = true;
	_lineSegment    tempLineSegment;

	for (auto circleItr : circlePoints)
	{
		if (startPoint)
		{
			tempLineSegment.start = circleItr;
			startPoint = !startPoint;
			continue;
		}
		tempLineSegment.end = circleItr;
		lineSegments.push_back(tempLineSegment);
		tempLineSegment.start = circleItr;
	}
	tempLineSegment.end = circlePoints[0];
	lineSegments.push_back (tempLineSegment);
}

//-----------------------------------------------------------------------------
//
// Draw a 2D quad
void gl_draw2DQuad ( string whichShader, GLuint whichTexture, float interpolation )
//-----------------------------------------------------------------------------
{
	static GLuint vao = 0;
	static GLuint buffers[2];
	static bool initDone = false;

	if ( !initDone )
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
		GL_CHECK (glBindVertexArray (vao));

		// Create buffers for the vertex data
		buffers[0] = wrapglGenBuffers (1, __func__);
		buffers[1] = wrapglGenBuffers (1, __func__);

		GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[0]));

		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof(_tile) * tiles.size(), &tiles[0], GL_DYNAMIC_DRAW));

		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof (_tile), (const GLvoid *) (offsetof (_tile, position))));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));

		// Texture coordinates buffer
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof (_tile), (const GLvoid *) (offsetof(_tile, textureCoords))));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));

		glBindVertexArray(0);

		initDone = true;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

	glUniformMatrix4fv (glGetUniformLocation (gl_getShaderID (whichShader), "MVP_Matrix"), 1, GL_FALSE, glm::value_ptr (MVP));
	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));

	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));

	GL_CHECK (glDrawArrays (GL_TRIANGLES, 0, 6 * tiles.size()));

	glBindVertexArray(0);

//	glDeleteBuffers (2, buffers);
//	glDeleteVertexArrays (1, &vao);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_createTileGeometry(glm::vec2 position, float tileSize, int tileID, int textureID)
//-----------------------------------------------------------------------------------------------------
{
	_tile tempTile;

	// Point zero - Triangle one
	tempTile.position.x = position.x - (tileSize * 0.5f);
	tempTile.position.y = position.y - (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point zero - Triangle one
	tempTile.textureCoords.x = 0.0f;
	tempTile.textureCoords.y = 0.0f;
	tiles.push_back(tempTile);

	// Point one - Triangle one
	tempTile.position.x = position.x + (tileSize * 0.5f);
	tempTile.position.y = position.y - (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point one - Triangle one
	tempTile.textureCoords.x = 1.0f;
	tempTile.textureCoords.y = 0.0f;
	tiles.push_back (tempTile);

	// Point two - Triangle one
	tempTile.position.x = position.x - (tileSize * 0.5f);
	tempTile.position.y = position.y + (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point two - Triangle two
	tempTile.textureCoords.x = 0.0f;
	tempTile.textureCoords.y = 1.0f;
	tiles.push_back (tempTile);

	// Point zero - Triangle two
	tempTile.position.x = position.x - (tileSize * 0.5f);
	tempTile.position.y = position.y + (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point zero - Triangle two
	tempTile.textureCoords.x = 0.0f;
	tempTile.textureCoords.y = 1.0f;
	tiles.push_back (tempTile);

	// Point one - Triangle two
	tempTile.position.x = position.x + (tileSize * 0.5f);
	tempTile.position.y = position.y - (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point one - Triangle two
	tempTile.textureCoords.x = 1.0f;
	tempTile.textureCoords.y = 0.0f;
	tiles.push_back (tempTile);

	// Point two - Triangle two
	tempTile.position.x = position.x + (tileSize * 0.5f);
	tempTile.position.y = position.y + (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point two - Triangle two
	tempTile.textureCoords.x = 1.0f;
	tempTile.textureCoords.y = 1.0f;
	tiles.push_back(tempTile);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_getLineSegments()
//-----------------------------------------------------------------------------------------------------
{
	vec3    point_0;
	vec3    point_1;
	vec3    point_2;
	vec3    point_3;
	_lineSegment    tempLineSegment;

	int baseIndex = 0;

	int count = 0;

	for (auto itr : tiles)
	{
		point_0 = tiles[baseIndex].position;
		uniquePoints.push_back(point_0);
		baseIndex += 1;

		point_1 = tiles[baseIndex].position;
		uniquePoints.push_back (point_1);
		baseIndex += 4;


		point_2 = tiles[baseIndex].position;
		uniquePoints.push_back (point_2);
		baseIndex -= 2;

		point_3 = tiles[baseIndex].position;
		uniquePoints.push_back (point_3);
		baseIndex += 3;     // Next quad starting vertex


		tempLineSegment.start = point_0;
		tempLineSegment.end = point_1;
		lineSegments.push_back(tempLineSegment);

		tempLineSegment.start = point_1;
		tempLineSegment.end = point_2;
		lineSegments.push_back (tempLineSegment);

		tempLineSegment.start = point_2;
		tempLineSegment.end = point_3;
		lineSegments.push_back (tempLineSegment);

		tempLineSegment.start = point_3;
		tempLineSegment.end = point_0;
		lineSegments.push_back (tempLineSegment);

		count++;
		if (count == tiles.size() / 6)
			return;
	}
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_addObjects()
//-----------------------------------------------------------------------------------------------------
{
	gam_createTileGeometry (vec2 (200, 200), 32, 0, 1);
	gam_createTileGeometry (vec2 (440, 400), 32, 0, 1);
	gam_createTileGeometry (vec2 (200, 240), 32, 0, 1);
	gam_createTileGeometry (vec2 (340, 340), 96, 0, 1);

	gam_createTileGeometry (vec2 (400, 300), 500, 0, 1);

	objectsAdded = true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_getUniqueAngles()
//-----------------------------------------------------------------------------------------------------
{
	vec3    tempMouse;

	tempMouse = io_getMousePosition ();

	for (auto itr : uniquePoints)
	{
		double angle = atan2(itr.y - tempMouse.y, itr.x - tempMouse.x);
		uniqueAngles.push_back (angle - 0.0001f);
		uniqueAngles.push_back (angle);
		uniqueAngles.push_back (angle + 0.0001f);
	}
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_drawAllObjects(float interpolate)
//-----------------------------------------------------------------------------------------------------
{
	vec3        tempMouse;

	tempMouse = io_getMousePosition ();

	tiles.clear();
	lineSegments.clear();
	uniquePoints.clear();
	uniqueAngles.clear();
	shadowHull.clear();
	circlePoints.clear();

	gl_set3DMode (interpolate);

	gam_addObjects ();
	gam_getLineSegments ();

	DrawCircle (tempMouse.x, tempMouse.y, 220, 32 );

	for (auto itr: lineSegments)
	{
		gl_drawLine (itr.start, itr.end, "colorLine", vec4 (1, 1, 1, 1));
	}

	gam_getUniqueAngles ();
	gam_doAngleRays ();
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
glm::vec3 gam_getIntersectPoint(_lineSegment ray, _lineSegment segment, bool *intersectFound, float *param)
//-----------------------------------------------------------------------------------------------------
{
	glm::vec3   returnValue{0,0,0};

	*intersectFound = false;

	// RAY in parametric: Point + Direction*T1
	float r_px = ray.start.x;
	float r_py = ray.start.y;
	float r_dx = ray.end.x - ray.start.x;
	float r_dy = ray.end.y - ray.start.y;

	// SEGMENT in parametric: Point + Direction*T2
	float s_px = segment.start.x;
	float s_py = segment.start.y;
	float s_dx = segment.end.x - segment.start.x;
	float s_dy = segment.end.y - segment.start.y;

	// Are they parallel? If so, no intersect
	float r_mag = sqrt (r_dx * r_dx + r_dy * r_dy);
	float s_mag = sqrt (s_dx * s_dx + s_dy * s_dy);

	if ( ((r_dx / r_mag) == (s_dx / s_mag)) && ((r_dy / r_mag) == (s_dy / s_mag)) )
	{ // Directions are the same.
		*intersectFound = false;
		return glm::vec3(0,0,0);
	}

	// SOLVE FOR T1 & T2
	// r_px+r_dx*T1 = s_px+s_dx*T2 && r_py+r_dy*T1 = s_py+s_dy*T2
	// ==> T1 = (s_px+s_dx*T2-r_px)/r_dx = (s_py+s_dy*T2-r_py)/r_dy
	// ==> s_px*r_dy + s_dx*T2*r_dy - r_px*r_dy = s_py*r_dx + s_dy*T2*r_dx - r_py*r_dx
	// ==> T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx)
	float T2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
	float T1 = (s_px + s_dx * T2 - r_px) / r_dx;

	// Must be within parametic whatevers for RAY/SEGMENT
	if ( T1 < 0.0f )
	{
		*intersectFound = false;
		return glm::vec3 (0,0,0);
	}
	if ( T2 < 0.0f || T2 > 1.0f )
	{
		*intersectFound = false;
		return glm::vec3 (0,0,0);
	}

	// Return the POINT OF INTERSECTION
	*intersectFound = true;
	*param = T1;
	returnValue =  glm::vec3(r_px + r_dx * T1, r_py + r_dy * T1, 0.0f);
	return returnValue;
//	return {x: r_px + r_dx * T1, y: r_py + r_dy * T1, param: T1};
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_drawCrossAtPoint(glm::vec3 point)
//-----------------------------------------------------------------------------------------------------
{
#define lineSize 6.0f

	gl_drawLine (vec3 (point.x - (lineSize * 0.5), point.y, point.z), vec3 (point.x + (lineSize * 0.5), point.y, point.z), "colorLine",
	             vec4 (0, 1, 1, 1));
	gl_drawLine (vec3 (point.x, point.y - (lineSize * 0.5), point.z), vec3 (point.x, point.y + (lineSize * 0.5), point.z), "colorLine",
	             vec4 (0, 1, 1, 1));

}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_doAngleRays()
//-----------------------------------------------------------------------------------------------------
{
	bool foundInterset = false;
	float param = 0.0f;

	glm::vec3 intersectPoint, closestIntersect;
	double dx, dy;
	_lineSegment mouseRay;

	for (auto angleItr : uniqueAngles)
	{
		dx = cos(angleItr);
		dy = sin(angleItr);

		mouseRay.start = io_getMousePosition ();
		mouseRay.end.x = mouseRay.start.x + dx;
		mouseRay.end.y = mouseRay.start.y + dy;
		mouseRay.end.z = 0.0f;

		float closestIntersectParam = 10000.0f;

		for ( const auto &itr : lineSegments )
		{
			intersectPoint = gam_getIntersectPoint (mouseRay, itr, &foundInterset, &param);

			if (!foundInterset)
				continue;

			if ( param < closestIntersectParam)
			{
				closestIntersect = intersectPoint;
				closestIntersectParam = param;
			}
		}
		foundInterset = false;

		_shadowHullPoint tempHullPoint;
		tempHullPoint.angle = angleItr;
		tempHullPoint.position = closestIntersect;

		shadowHull.insert(tempHullPoint);
		}

	gl_drawShadowHull (shadowHull, io_getMousePosition (), "colorLine");

	for (auto hullPointItr : shadowHull)
	{
		gl_drawLine (mouseRay.start, hullPointItr.position, "colorLine", vec4 (1, 0, 0, 1));
		gam_drawCrossAtPoint (hullPointItr.position);
	}
}