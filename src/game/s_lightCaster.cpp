#include "hdr/system/sys_main.h"
#include <hdr/libGL/glm/glm.hpp>
#include <hdr/game/s_levels.h>
#include <hdr/opengl/gl_openGLWrap.h>
#include "s_lightCaster.h"
#include "hdr/opengl/gl_shaders.h"

typedef struct {
	glm::vec3       start;
	glm::vec3       end;
} _lightLineSegment;

vector<glm::vec3>           lightUniquePoints;
vector<double>              lightUniqueAngles;
vector<_lightLineSegment>   lightLineSegments;
set<_lightHullPoint>        lightHull;
vector<glm::vec3>           circlePoints;

vec3    lightPosition;

void DrawCircle ( float cx, float cy, float r, int num_segments )
{
	auto theta = static_cast<float>(2 * 3.1415926 / float (num_segments));
	float c = cosf (theta);//precalculate the sine and cosine
	float s = sinf (theta);
	float t;

	float x = r; // Start at angle = 0
	float y = 0;

	for ( int ii = 0; ii < num_segments; ii++ )
	{
		circlePoints.push_back(glm::vec3(x + cx, y+cy, 0.0f));
		lightUniquePoints.push_back (glm::vec3 (x + cx, y + cy, 0.0f));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}

	bool startPoint = true;
	_lightLineSegment    tempLineSegment;

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
void light_getUniqueAngles()
//-----------------------------------------------------------------------------------------------------
{
	for (auto itr : lightUniquePoints) //levelInfo.at(currentLevelName).lineSegments)
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
void glight_drawShadowHull ( std::set<_lightHullPoint> const &drawShadowHull, glm::vec3 casterPosition, string whichShader )
//-----------------------------------------------------------------------------
{
	static GLuint vao = 0;
	static GLuint buffers;
	static bool initDone = false;

	glm::vec4 debugColor{0.8,0.8,0.8,0.5};

	vector<glm::vec3>       hullPoints;

	//
	// Create the triangle fan from the already sorted drawShadowHull
	//
	hullPoints.clear();
	hullPoints.push_back (casterPosition);
	for (auto sourceItr : drawShadowHull)
	{
		hullPoints.push_back(sourceItr.position);
	}

	auto sourceItr = drawShadowHull.begin();
	hullPoints.push_back (sourceItr->position);

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
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (glm::vec3) * hullPoints.size(), &hullPoints[0], GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = false;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glUniform4fv                 (gl_getUniform (whichShader, "inColor"), 1, glm::value_ptr (debugColor)));
	GL_CHECK (glUniformMatrix4fv           (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));
	GL_CHECK (glEnableVertexAttribArray    (gl_getAttrib  (whichShader, "inPosition")));

	GL_CHECK (glDrawArrays (GL_TRIANGLE_FAN, 0, hullPoints.size ()));

	glDeleteBuffers (1, &buffers);
	glDeleteVertexArrays (1, &vao);
}


//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void light_doAngleRays()
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

		lightHull.insert(tempHullPoint);
	}

	glight_drawShadowHull (lightHull, lightPosition, "colorLine");

	for (auto hullPointItr : lightHull)
	{
//		gl_drawLine (lightRay.start, hullPointItr.position, "colorLine", vec4 (1, 0, 0, 0.9));
//		gam_drawCrossAtPoint (hullPointItr.position);
	}
}



void do_light()
{
	_lightLineSegment       tempLightLineSegment;

	lightLineSegments.clear();
	lightUniquePoints.clear();
	lightUniqueAngles.clear();
	lightHull.clear();
	circlePoints.clear();

	vec3    tempPoint;

	DrawCircle ( lightPosition.x, lightPosition.y, 100.0f, 32 );

	for (int i = 0; i < levelInfo.at(currentLevelName).lineSegments.size(); i++)
	{
		tempLightLineSegment.start.x = static_cast<float>(levelInfo.at(currentLevelName).lineSegments[i].x);
		tempLightLineSegment.start.y = static_cast<float>(levelInfo.at(currentLevelName).lineSegments[i].y);
		tempLightLineSegment.start.z = 0.0f;

		tempPoint.x = tempLightLineSegment.start.x;
		tempPoint.y = tempLightLineSegment.start.y;

		lightUniquePoints.push_back(tempPoint);
		i++;

		tempLightLineSegment.end.x = static_cast<float>(levelInfo.at(currentLevelName).lineSegments[i].x);
		tempLightLineSegment.end.y = static_cast<float>(levelInfo.at(currentLevelName).lineSegments[i].y);
		tempLightLineSegment.end.z = 0.0f;

		tempPoint.x = tempLightLineSegment.end.x;
		tempPoint.y = tempLightLineSegment.end.y;

		lightUniquePoints.push_back(tempPoint);

		lightLineSegments.push_back(tempLightLineSegment);
	}

	/*
	for (auto itr: lightLineSegments)
	{
		gl_drawLine (itr.start, itr.end, "colorLine", vec4 (1, 0, 1, 1));
		printf("segment start [ %f %f ] end [ %f %f ]\n", itr.start.x, itr.start.y, itr.end.x, itr.end.y);
	}
*/
	light_getUniqueAngles();

	light_doAngleRays();
}