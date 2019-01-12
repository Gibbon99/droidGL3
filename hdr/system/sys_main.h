#pragma once

#include <stdio.h>
#include <SDL.h>
#include <string>
#include <vector>
#include <utility>
#include <queue>
#include <unordered_map>
#include <cstdlib>
#include "hdr/libGL/glad/glad.h"

//#include "hdr/system/sys_leakDetector.h"

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "hdr/libGL/chipmunk/cpVect.h"

#include "hdr/system/sys_defines.h"
#include "hdr/script/angelscript.h"
#include "hdr/script/scriptstdstring.h"
#include "hdr/script/as_scriptarray.h"

#include "hdr/system/sys_shutdown.h"
#include "hdr/console/con_console.h"
#include "hdr/system/sys_events.h"

#include "data/scripts/commonDefines.h"

using namespace std;
using namespace glm;

const int TICKS_PER_SECOND = 30;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

extern Uint32 currentServerTick;
extern Uint32 currentClientTick;
extern Uint32 networkServerTick;

extern Uint32 frameStart;
extern Uint32 frameTime;
extern Uint32 frameCount;

extern bool         quitProgram;
extern int          fps, thinkFPS;
extern int          winWidth, winHeight;    // Screen size
extern int          currentMode;
extern bool         fullScreen;
extern int          vsyncType;
extern cpVect       viewableScreenCoord;
extern float        interpolation;

extern vec3 quadPosition;

// Change game mode
void sys_changeMode ( int newMode );
