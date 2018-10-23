cmake_minimum_required(VERSION 3.10)
project(droidGL)

set(CMAKE_CXX_STANDARD 14)

# Set default locations
set(CL_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})

set(CONFIGURATION_NAME "Debug")

include_directories(
        .
        hdr
        hdr/console
        hdr/game
        hdr/io
        hdr/libGL
        hdr/libGL/glad
        hdr/opengl
        hdr/script
        hdr/system
        hdr/physfs
        hdr/libGL/freetype-gl
        hdr/libGL/chipmunk
        hdr/libGL/soil
        /usr/include/freetype2
        /usr/local/include/SDL2
)

# Library path
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L. -L/usr/local/lib ")

# Define the CXX sources
set ( CXX_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_leakDetector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_shutdown.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_startup.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_events.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_embedFont.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_audio.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_openGLWrap.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_opengl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/game/s_gameEvents.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/game/s_gamePhysics.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/gl_window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/glad/glad.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/io/io_keyboard.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/io/io_logfile.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/io/io_fileSystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/console/con_console.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/console/con_comHelp.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/console/con_conScript.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/console/con_openGLInfo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/console/con_quit.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/console/con_variables.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_timing.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptengine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_objecttype.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_arm.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_atomic.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/scriptstdstring.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_configgroup.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptcode.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_memory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_tokenizer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptfunction.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_bytecode.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_builder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_x64_gcc.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptnode.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_gc.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_string_util.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_restore.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_typeinfo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_x64_msvc.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_globalproperty.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_x64_mingw.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_xenon.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptobject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_mips.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_thread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_x86.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/scriptstdstring_utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_outputbuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_datatype.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_string.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_variablescope.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_compiler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_module.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptarray.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_ppc_64.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_sh4.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_scriptbuilder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_generic.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_parser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/script/as_callfunc_ppc.cpp

        ${CMAKE_CURRENT_LIST_DIR}/src/io/io_textures.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/io/io_mouse.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_fbo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_shaders.cpp
        src/io/io_gameprefs.cpp hdr/io/io_gameprefs.h src/game/s_levels.cpp hdr/game/s_levels.h src/game/s_render.cpp hdr/game/s_render.h src/game/s_movement.cpp hdr/game/s_movement.h src/game/s_renderDebug.cpp hdr/game/s_renderDebug.h src/game/s_lightCaster.cpp hdr/game/s_lightCaster.h src/opengl/gl_renderSprite.cpp hdr/opengl/gl_renderSprite.h src/game/s_hud.cpp hdr/game/s_hud.h src/game/s_healing.cpp hdr/game/s_healing.h src/game/s_player.cpp hdr/game/s_player.h src/game/s_game.cpp hdr/game/s_game.h src/game/s_lifts.cpp hdr/game/s_lifts.h)

# Define the C sources
set(C_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/distance-field.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/font-manager.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/text-buffer.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/texture-atlas.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/texture-font.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/platform.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/utf8-utils.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/vector.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/edtaa3func.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/vertex-buffer.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/freetype-gl/vertex-attribute.c

        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_iso9660.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_zip.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_unicode.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_platform_qnx.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_mvl.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_unpacked.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_platform_unix.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_hog.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_slb.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_platform_windows.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_platform_posix.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_platform_os2.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_byteorder.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_wad.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_vdf.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_qpak.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_grp.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_7z.c
        ${CMAKE_CURRENT_LIST_DIR}/src/physfs/physfs_archiver_dir.c

        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/chipmunk.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpArbiter.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpArray.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpBBTree.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpBody.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpCollision.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpConstraint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpDampedRotarySpring.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpDampedSpring.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpGearJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpGrooveJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpHashSet.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpHastySpace.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpMarch.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpPinJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpPivotJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpPolyline.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpPolyShape.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpRatchetJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpRobust.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpRotaryLimitJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpShape.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSimpleMotor.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSlideJoint.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpace.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpaceComponent.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpaceDebug.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpaceHash.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpaceQuery.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpaceStep.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSpatialIndex.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/chipmunk/cpSweep1D.c

        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/soil/image_DXT.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/soil/image_helper.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/soil/soil.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/soil/stb_image_aug.c
        ${CMAKE_CURRENT_LIST_DIR}/src/io/simpleIni/io_convertUTF.c )

set_source_files_properties(
        ${CXX_SRCS} PROPERTIES COMPILE_FLAGS
        " -pg -g -O0")

set_source_files_properties(
        ${C_SRCS} PROPERTIES COMPILE_FLAGS
        " -g -O0 -Wall")

add_executable(droidGL ${RC_SRCS} ${CXX_SRCS} ${C_SRCS})
target_link_libraries(droidGL ${LINK_OPTIONS})


target_link_libraries(droidGL
        GL
        GLU
        glload
        rt
        pthread
        dl
        SDL2
        SDL2_mixer
        freetype
        )