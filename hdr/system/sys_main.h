#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <utility>
#include <queue>
#include "hdr/libGL/glad/glad.h"

// Include GLM
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtx/transform.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "../glm/gtx/intersect.hpp"
#include "../glm/gtc/matrix_inverse.hpp"

#include "hdr/system/sys_defines.h"
#include "hdr/script/angelscript.h"
#include "hdr/script/scriptstdstring.h"
#include "hdr/script/as_scriptarray.h"

#include "hdr/system/sys_shutdown.h"
#include "hdr/console/con_console.h"
#include "hdr/system/sys_events.h"

using namespace std;
using namespace glm;

const int TICKS_PER_SECOND = 30;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

extern bool quitProgram;
extern int  fps, thinkFPS;
extern int  winWidth, winHeight;    // Screen size
extern int  currentMode;

// Change game mode
void changeMode ( int newMode );