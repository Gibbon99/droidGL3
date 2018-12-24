#include "hdr/system/sys_main.h"
#include "hdr/libGL/glm/glm.hpp"
#include "hdr/game/gam_levels.h"
#include "hdr/opengl/gl_openGLWrap.h"
#include "hdr/opengl/gl_fbo.h"
#include "hdr/game/gam_lightCaster.h"
#include "hdr/opengl/gl_shaders.h"
#include "hdr/io/io_textures.h"

Uint64      startTime;
double      deltaTime;

bool         g_debugShowHullCircle;
bool         g_debugShowHullLines;

typedef struct
{
	glm::vec3       start;
	glm::vec3       end;
} _lightLineSegment;

vector<glm::vec3>           lightUniquePoints;
vector<double>              lightUniqueAngles;
vector<_lightLineSegment>   lightLineSegments;
set<_lightHullPoint>        lightHull;
vector<glm::vec3>           circlePoints;

vector<_lightCaster>        lightCasters;

vector<_lightLineSegment>   copyLineSegments;

//-------------------------------------------------------------------------------
//
// Add a new light caster for this level
//
// Pass 0 to radius to delete all texture handles and clear vector
void light_addNewCaster(const glm::vec3 position, const glm::vec3 color, GLuint radius)
//-------------------------------------------------------------------------------
{
	_lightCaster        tempLightCaster;

	if (radius == 0)
	{
		if (!lightCasters.empty())
		{
			for (const auto &lightCastersItr : lightCasters)
			{
				glDeleteTextures(1, &lightCastersItr.textureID);
			}
		}
		lightCasters.clear();       // Reset and clear
	}


	tempLightCaster.position = position;
	tempLightCaster.color = color;
	tempLightCaster.radius = radius;
	tempLightCaster.textureID = gl_createNewTexture(256, 256);

	lightCasters.push_back(tempLightCaster);
}

//-------------------------------------------------------------------------------
//
// Draw a circle to generate the line segments used to test ray intersections
// so we get a circular shape even if there is no line segment points near.
//
// Pass in the center of the circle ( same as the light ) - the radius
// and how many line segments are generate to make the shape.
//
// Line segments and points are added to the lightLineSegment array
void light_createCircleSegmentsAndUniquePoints ( float cx, float cy, float r, int num_segments )
//-------------------------------------------------------------------------------
{
	_lightLineSegment    tempLineSegment;

	auto theta = static_cast<float>(2 * 3.1415926 / float (num_segments));

	float c = cosf (theta);     // pre calculate the sine and cosine
	float s = sinf (theta);
	float t;

	float x = r; // Start at angle = 0
	float y = 0;

	for ( auto i = 0; i < num_segments; i++ )
	{
		circlePoints.push_back(glm::vec3(x + cx, y+cy, 0.0f));
		//
		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}

	bool startPoint = true;

	for ( const auto &circleItr : circlePoints)
	{
		if (startPoint)
		{
			tempLineSegment.start = circleItr;
			startPoint = !startPoint;
			continue;
		}
		tempLineSegment.end = circleItr;
		lightLineSegments.push_back(tempLineSegment);

		lightUniquePoints.push_back(tempLineSegment.start);
		lightUniquePoints.push_back(tempLineSegment.end);

		tempLineSegment.start = circleItr;
	}
	tempLineSegment.end = circlePoints[0];
	lightLineSegments.push_back (tempLineSegment);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void light_createAnglesFromPoints (vec3 lightPosition)
//-----------------------------------------------------------------------------------------------------
{
	for (auto itr : lightUniquePoints)
	{
		double angle = atan2(itr.y - lightPosition.y, itr.x - lightPosition.x);
		lightUniqueAngles.push_back (angle - 0.001f);
		lightUniqueAngles.push_back (angle);
		lightUniqueAngles.push_back (angle + 0.001f);
	}
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
glm::vec3 light_getIntersectPoint(_lightLineSegment ray, _lightLineSegment segment, bool *intersectFound, float *param)
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

//-----------------------------------------------------------------------------
//
// Draw the shadowHull
void light_createShadowTexture ( std::set<_lightHullPoint> const &drawShadowHull, const glm::vec3 casterPosition,
                                 const string whichShader )
//-----------------------------------------------------------------------------
{
	static GLuint vao = 0;
	static GLuint buffers;
	static bool initDone = false;

	glm::vec4 debugColor{1.0f, 1.0f, 1.0f, 1.0f};

	typedef struct
	{
		glm::vec3   position;
	} _texHullPoints;

	vector<_texHullPoints>       hullPoints;

	static glm::vec3   scaleValue;

	if (0 == io_getTextureID ("lightcaster"))
	{
		glm::vec2 fullSize;

		fullSize = io_getTextureSize ("fullLevelTexture");
		io_storeTextureInfoIntoMap(gl_createNewTexture (256, 256), glm::vec2{256, 256}, "lightcaster", false);

		scaleValue.x = fullSize.x / 256;
		scaleValue.y = fullSize.y / 256;
		scaleValue.z = 0.0f;

		printf("Scale x [ %3.3f ] Y [ %3.3f ]\n", scaleValue.x, scaleValue.y);
	}


	//
	// Create the triangle fan from the already sorted drawShadowHull
	//
	hullPoints.clear();

	_texHullPoints  tempTexHullPoints;

	// Reuse drawShadowHull - just insert at the start and the end??

	tempTexHullPoints.position = casterPosition;
	hullPoints.push_back (tempTexHullPoints);

	for (auto sourceItr : drawShadowHull)
	{
		tempTexHullPoints.position.x = sourceItr.position.x;
		tempTexHullPoints.position.y = sourceItr.position.y;
		hullPoints.push_back(tempTexHullPoints);
	}

	auto sourceItr = drawShadowHull.begin();

	tempTexHullPoints.position = sourceItr->position;
	hullPoints.push_back(tempTexHullPoints);

	if ( !initDone )
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
		GL_CHECK (glBindVertexArray (vao));

		// Create buffers for the vertex data
		buffers = wrapglGenBuffers (1, __func__);

		GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (_texHullPoints) * hullPoints.size(), &hullPoints[0].position, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof (_texHullPoints), (GLvoid *)offsetof(_texHullPoints, position)));

		initDone = false;
	}




	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

	if (!gl_renderToFrameBuffer ("lightcaster"))
	{
		printf("Unable to use frame buffer.\n");
	}

//	gl_set2DMode ( 256, 256, scaleValue);

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
//	GL_CHECK (glUniform4fv                 (gl_getUniform (whichShader, "inColor"), 1, glm::value_ptr (debugColor)));
	GL_CHECK (glUniformMatrix4fv           (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));
	GL_CHECK (glEnableVertexAttribArray    (gl_getAttrib  (whichShader, "inPosition")));
	//
	// Create the black and white hull mask texture
	//
	GL_CHECK (glDrawArrays (GL_TRIANGLE_FAN, 0, hullPoints.size ()));

	glDeleteBuffers (1, &buffers);
	glDeleteVertexArrays (1, &vao);

//	gl_renderToScreen ();
}


//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void light_createLightHull (vec3 lightPosition)
//-----------------------------------------------------------------------------------------------------
{
	bool    foundIntersect = false;
	float   param = 0.0f;

	glm::vec3           intersectPoint, closestIntersect;
	double              dx, dy;
	_lightLineSegment   lightRay;

	for (auto angleItr : lightUniqueAngles)
	{
		dx = cos(angleItr);
		dy = sin(angleItr);

		lightRay.start = lightPosition;
		lightRay.end.x = static_cast<float>(lightRay.start.x + dx);
		lightRay.end.y = static_cast<float>(lightRay.start.y + dy);
		lightRay.end.z = 0.0f;

		float closestIntersectParam = 10000.0f;

		for ( const auto &itr : lightLineSegments )
		{
			intersectPoint = light_getIntersectPoint (lightRay, itr, &foundIntersect, &param);

			if (!foundIntersect)
				continue;

			if ( param < closestIntersectParam)
			{
				closestIntersect = intersectPoint;
				closestIntersectParam = param;
			}
		}
		foundIntersect = false;

		_lightHullPoint tempHullPoint;
		tempHullPoint.angle = angleItr;
		tempHullPoint.position = closestIntersect;

//		tempHullPoint.texCoord.x = static_cast<float>(std::cos(angleItr) + 1.0)*0.5;
//		tempHullPoint.texCoord.y = static_cast<float>(std::sin(angleItr) + 1.0)*0.5;

		lightHull.insert(tempHullPoint);
	}
}

//-----------------------------------------------------------------------------------------------------
//
// Create the line segments used to calculate the light ray intersections
void light_createLineSegmentsAndUniquePoints (string levelName)
//-----------------------------------------------------------------------------------------------------
{
	_lightLineSegment       tempLightLineSegment;
	vec3                    tempPoint;

	for (int i = 0; i < levelInfo.at(levelName).lineSegments.size(); i++)
	{
		tempLightLineSegment.start.x = static_cast<float>(levelInfo.at(levelName).lineSegments[i].x);
		tempLightLineSegment.start.y = static_cast<float>(levelInfo.at(levelName).lineSegments[i].y);
		tempLightLineSegment.start.z = 0.0f;

		tempPoint.x = tempLightLineSegment.start.x;
		tempPoint.y = tempLightLineSegment.start.y;

		lightUniquePoints.push_back(tempPoint);
		i++;

		tempLightLineSegment.end.x = static_cast<float>(levelInfo.at(levelName).lineSegments[i].x);
		tempLightLineSegment.end.y = static_cast<float>(levelInfo.at(levelName).lineSegments[i].y);
		tempLightLineSegment.end.z = 0.0f;

		tempPoint.x = tempLightLineSegment.end.x;
		tempPoint.y = tempLightLineSegment.end.y;

		lightUniquePoints.push_back(tempPoint);

		lightLineSegments.push_back(tempLightLineSegment);

		copyLineSegments = lightLineSegments;
	}
}

void light_createLightCaster (string levelName, vec3 lightPosition)
{
	startTime = SDL_GetPerformanceCounter ();

	lightLineSegments.clear();
	lightUniquePoints.clear();
	lightUniqueAngles.clear();
	lightHull.clear();
	circlePoints.clear();
	//
	// Make a copy of the loaded level line segments
	light_createLineSegmentsAndUniquePoints (levelName);
	//
	// Create line segments used to bound the light
	light_createCircleSegmentsAndUniquePoints (lightPosition.x, lightPosition.y, 70.0f, 36);

	light_createAnglesFromPoints (lightPosition);

	light_createLightHull (lightPosition);

//	light_createShadowTexture (lightHull, lightPosition, "colorLine");
	light_createShadowTexture (lightHull, lightPosition, "quad3d");

	if (g_debugShowHullCircle)
	{
		for ( auto itr: lightLineSegments )
		{
			gl_drawLine (itr.start, itr.end, "colorLine", vec4 (1, 0, 1, 1));
		}
	}

	if (g_debugShowHullLines)
	{
		for ( auto const &hullPointItr : lightHull )
		{
			gl_drawLine (lightPosition, hullPointItr.position, "colorLine", vec4 (1, 0, 0, 0.9));
		}
	}

	deltaTime = (SDL_GetPerformanceCounter () - startTime) * 1000 / (double)SDL_GetPerformanceFrequency ();
}
