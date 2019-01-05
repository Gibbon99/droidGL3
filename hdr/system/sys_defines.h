#pragma once

// Current function macro.
#ifdef WIN32
#define __current__func__ __FUNCTION__
#else
#define __current__func__ __func__
#endif

// Assert macros.
#ifdef _DEBUG
#define GP_ASSERT(expression) assert(expression)
#else
#define GP_ASSERT( expression )
#endif

// Global variable to hold GL errors
extern int __gl_error_code;

/**
 * GL assertion that can be used for any OpenGL function call.
 *
 * This macro will assert if an error is detected when executing
 * the specified GL code. This macro will do nothing in release
 * mode and is therefore safe to use for realtime/per-frame GL
 * function calls.
 */
#ifdef NDEBUG
#define GL_ASSERT( gl_code ) gl_code
#else
#define GL_ASSERT( gl_code ) do \
        { \
            gl_code; \
            __gl_error_code = glGetError(); \
            GP_ASSERT(__gl_error_code == GL_NO_ERROR); \
        } while(0)
#endif


// Error macro.
#define GL_ERROR( ... ) do \
        { \
            printf("%s -- ", __current__func__); \
            printf("\n"); \
        } while (0)

/**
 * Executes the specified GL code and checks the GL error afterwards
 * to ensure it succeeded.
 *
 * This macro should be used instead of GL_ASSERT for code that must
 * be checked in both debug and release builds. The GL_LAST_ERROR
 * macro can be used afterwards to check whether a GL error was
 * encountered executing the specified code.
 */
#define GL_CHECK( gl_code ) do \
        { \
            while (glGetError() != GL_NO_ERROR) ; \
            gl_code; \
            __gl_error_code = glGetError(); \
            if (__gl_error_code != GL_NO_ERROR) \
                { \
                    gl_getAllGLErrors ( __gl_error_code, __current__func__, __LINE__ ); \
                } \
        } while(0)

/**
 * Accesses the most recently set global GL error.
 */
#define GL_LAST_ERROR() __gl_error_code

//
// How many characters to store in a string - usually logfile or console
#define     MAX_STRING_SIZE     128
//
// How many fonts are defined
#define     MAX_NUM_FONTS       3

#define     FONT_SMALL          0
#define     FONT_MEDIUM         1
#define     FONT_LARGE          2

#define BUFFER_OFFSET( i ) ((char *)NULL + (i))

#define     GLSL_SHADER         0
#define     GLSL_PROGRAM        1

#define CON_NOCHANGE    0   // Don't change the console text color
#define CON_TEXT        1
#define CON_INFO        2
#define CON_ERROR       3

//----------------------------------------------------
//
// Define types used for set/get variables in host
//
//----------------------------------------------------
#define VAR_TYPE_INT        0
#define VAR_TYPE_STRING        1
#define VAR_TYPE_VEC3        2
#define VAR_TYPE_TEXT       3
#define VAR_TYPE_BOOL       4
#define VAR_TYPE_FLOAT      5

#define TTF_FILENAME_SIZE   32

//-----------------------------------------------------------------------------
//
// Name of ini file to hold startup values
//
//-----------------------------------------------------------------------------

#define GAME_PREFS_FILENAME     "data/gameprefs.ini"

#define SPRITE_SIZE   24

enum bulletTypes
{
    BULLET_TYPE_NORMAL = 0,
    BULLET_TYPE_SINGLE,
    BULLET_TYPE_DOUBLE,
    BULLET_TYPE_DISRUPTER
};

#if defined _WIN32_
	#define uint unsigned int
#endif
