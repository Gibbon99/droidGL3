cmake_minimum_required(VERSION 3.10)
project(droidGL)

set(CMAKE_CXX_STANDARD 14)

# Set default locations
set(CL_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})

set(CONFIGURATION_NAME "Debug")

if (DEBUG_LEVEL)
    add_definitions(-DDEBUG_LEVEL=${DEBUG_LEVEL})
endif ()

include_directories(
        .
#        hdr
        hdr/libGL
        hdr/libGL/glad
        hdr/script
        hdr/physfs
        hdr/libGL/freetype-gl
        hdr/libGL/chipmunk
        hdr/libGL/soil
        hdr/network/raknet
        /usr/include/freetype2
        /usr/include/SDL2
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
        ${CMAKE_CURRENT_LIST_DIR}/src/game/gam_events.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/game/gam_physics.cpp
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
        ${CMAKE_CURRENT_LIST_DIR}/src/io/io_gameprefs.cpp

        ${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_fbo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_shaders.cpp
		${CMAKE_CURRENT_LIST_DIR}/src/opengl/gl_renderSprite.cpp 

		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_levels.cpp
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_render.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_renderDebug.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_lightCaster.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_hud.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_healing.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_player.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_game.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_lifts.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_droids.cpp		
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_droidAIPatrol.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_droidNetwork.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_doors.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_physicsCollisions.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_physicsPlayer.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_eventsServer.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/game/gam_eventsClient.cpp

		${CMAKE_CURRENT_LIST_DIR}/src/system/sys_utils.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/system/sys_maths.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/network/net_server.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/network/net_client.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/network/net_common.cpp 

        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DynDNS.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/GridSectorizer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/FileListTransfer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DataCompressor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/LinuxStrings.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PacketLogger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/ReadyEvent.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/ReplicaManager3.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/BitStream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PacketOutputWindowLogger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/LocklessTypes.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CloudServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PS4Includes.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RelayPlugin.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Itoa.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/ReliabilityLayer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetStatistics.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NatPunchthroughServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/SendToThread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Gets.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DS_ByteQueue.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_Berkley.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DS_HuffmanEncodingTree.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/SecureHandshake.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/ConsoleServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/FullyConnectedMesh2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NatTypeDetectionCommon.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/TableSerializer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_Windows_Linux_360.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/UDPProxyServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/UDPProxyCoordinator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakPeer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DS_BytePool.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CloudClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_PS4.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/ThreadsafePacketLogger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/MessageFilter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CCRakNetUDT.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PacketizedTCP.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/TelnetTransport.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Base64Encoder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakThread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PacketConsoleLogger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/SimpleMutex.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetTransport2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Getche.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Rackspace.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakSleep.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RandSync.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/EpochTimeToString.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/VariableDeltaSerializer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakWString.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NetworkIDObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/HTTPConnection2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/LogCommandParser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CCRakNetSlidingWindow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_Windows_Linux.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/StatisticsHistory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NatTypeDetectionServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_NativeClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_Berkley_NativeClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/SuperFastHash.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/FileList.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/StringTable.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/StringCompressor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/TeamManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/TeamBalancer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetTypes.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PluginInterface2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/VitaIncludes.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CloudCommon.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_360_720.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/TCPInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CommandParserInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_PS3_PS4.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/VariadicSQLParser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/UDPProxyClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/ConnectionGraph2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NetworkIDManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NatPunchthroughClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/_FindFirst.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/CheckSum.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetCommandParser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/HTTPConnection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DR_SHA1.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/gettimeofday.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/EmailSender.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Router2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_WindowsStore8.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/IncrementalReadInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/SocketLayer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DS_Table.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakString.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/WSAStartupSingleton.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/FileOperations.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/DirectoryDeltaTransfer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/FormatString.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakMemoryOverride.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/SignaledEvent.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/NatTypeDetectionClient.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/PacketFileLogger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RakNetSocket2_Vita.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/GetTime.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/Rand.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/TwoWayAuthentication.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/UDPForwarder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/RPC4Plugin.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/network/raknet/VariableListDeltaTracker.cpp

        ${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/system/sys_sdfFont.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_button.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_render.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_checkBox.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_textBox.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_label.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/gui/gui_language.cpp
        src/gui/gui_scrollBox.cpp hdr/gui/gui_scrollBox.h src/gui/gui_sideview.cpp hdr/gui/gui_sideview.h)

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
        ${CMAKE_CURRENT_LIST_DIR}/src/io/simpleIni/io_convertUTF.c

        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/sdl2_gfx/SDL2_framerate.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/sdl2_gfx/SDL2_gfxPrimitives.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/sdl2_gfx/SDL2_imageFilter.c
        ${CMAKE_CURRENT_LIST_DIR}/src/libGL/sdl2_gfx/SDL2_rotozoom.c

        ${CMAKE_CURRENT_LIST_DIR}/src/gui/SDL_ttf.c
        )

set_source_files_properties(
        ${CXX_SRCS} PROPERTIES COMPILE_FLAGS
        " -pg -g -O0")
#        " -pg -g -O0 -fsanitize=address -fno-omit-frame-pointer")	# Address sanitizer - and memleak
#        " -pg -g -O0 -fsanitize=thread -fno-omit-frame-pointer")	# Thread sanitizer

set_source_files_properties(
        ${C_SRCS} PROPERTIES COMPILE_FLAGS
        " -g -O0 -Wall -Wpointer-sign")

add_executable(droidGL ${RC_SRCS} ${CXX_SRCS} ${C_SRCS})
target_link_libraries(droidGL ${LINK_OPTIONS})


target_link_libraries(droidGL
#        asan		# Used for address sanitizer
#		tsan		# Used for thread sanitizer
        GL
        GLU
        rt
        pthread
        dl
        SDL2
        SDL2_mixer
        freetype
        )
