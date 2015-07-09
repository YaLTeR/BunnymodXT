cmake_minimum_required (VERSION 3.1)
project (BunnymodXT)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
#set (CMAKE_CXX_STANDARD 11)

include(cotire)

if (MSVC)
	# Until there's a VS2015 redistributable, link the runtime statically.
	foreach (flag_var
    	CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
    	CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if (${flag_var} MATCHES "/MD")
			string (REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif (${flag_var} MATCHES "/MD")
	endforeach (flag_var)
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=native -mtune=native -m32 -g -Ofast -static-libstdc++ -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
	#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=pentium-m -mfpmath=387 -m32 -g -Ofast -static-libstdc++ -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
endif ()

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME ON)
find_package (Boost 1.57.0 REQUIRED COMPONENTS chrono date_time system thread)
include_directories (${Boost_INCLUDE_DIR})

add_subdirectory (hltas)
add_subdirectory (hlstrafe)

include_directories ("." hltas/src hlstrafe/src)

set (LIBRARY_OUTPUT_DIRECTORY ".")
set (WINDOWS_FILES
	SPTLib/Windows/DetoursUtils.cpp
	SPTLib/Windows/Hooks_win.cpp
	SPTLib/Windows/MemUtils_win.cpp
	BunnymodXT/Windows/conutils.cpp
	BunnymodXT/Windows/dllmain.cpp)
set (LINUX_FILES
	SPTLib/Linux/Hooks_linux.cpp
	SPTLib/Linux/MemUtils_linux.cpp
	BunnymodXT/modules/PMSharedHooks_linux.cpp
	BunnymodXT/Linux/main_linux.cpp)
set (SOURCE_FILES
	SPTLib/IHookableModule.cpp
	SPTLib/IHookableDirFilter.cpp
	SPTLib/IHookableNameFilter.cpp
	SPTLib/IHookableNameFilterOrdered.cpp
	SPTLib/Hooks.cpp
	SPTLib/MemUtils.cpp
	SPTLib/sptlib.cpp
	BunnymodXT/modules/HwDLL.cpp
	BunnymodXT/modules/ClientDLL.cpp
	BunnymodXT/modules/ServerDLL.cpp
	BunnymodXT/cvars.cpp
	BunnymodXT/hud_custom.cpp
	BunnymodXT/interprocess.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Find Detours.
	if (NOT DEFINED Detours_ROOT)
		message (FATAL_ERROR "Could not find Detours, point Detours_ROOT to the Detours base dir.")
	endif ()
	find_path (Detours_INCLUDE_DIR detours.h HINTS ${Detours_ROOT}/include)
	if (Detours_INCLUDE_DIR)
		message (STATUS "Found the Detours include directory at " ${Detours_INCLUDE_DIR})
	else ()
		message (FATAL_ERROR "Could not find the Detours include directory, point Detours_ROOT to the Detours base dir.")
	endif ()
	find_file (Detours_LIBRARY detours.lib HINTS ${Detours_ROOT}/lib.X86)
	if (Detours_LIBRARY)
		message (STATUS "Found the detours.lib at " ${Detours_LIBRARY})
	else ()
		message (FATAL_ERROR "Could not find detours.lib, point Detours_ROOT to the Detours base dir.")
	endif ()

	include_directories (${Detours_INCLUDE_DIR})
	set (SOURCE_FILES ${SOURCE_FILES} ${WINDOWS_FILES})
else ()
	set (SOURCE_FILES ${SOURCE_FILES} ${LINUX_FILES})
endif ()

add_library (BunnymodXT SHARED ${SOURCE_FILES})
set_target_properties(BunnymodXT PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "BunnymodXT/stdafx.hpp")
cotire(BunnymodXT)

target_link_libraries (BunnymodXT hltas hlstrafe ${Boost_LIBRARIES})
if (NOT MSVC)
	target_link_libraries (BunnymodXT pthread rt)
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries (BunnymodXT ${Detours_LIBRARY})
endif ()