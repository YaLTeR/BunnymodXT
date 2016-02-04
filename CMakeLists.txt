cmake_minimum_required (VERSION 3.1)
project (BunnymodXT)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(cotire)

if (NOT MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=native -mtune=native -m32 -g -Ofast -static-libstdc++ -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-variable")
	#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=pentium-m -mfpmath=387 -m32 -g -Ofast -static-libstdc++ -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-variable")
endif ()

set (Boost_USE_STATIC_LIBS ON)
find_package (Boost 1.57.0 REQUIRED COMPONENTS chrono date_time system thread)
include_directories (SYSTEM ${Boost_INCLUDE_DIR})

add_subdirectory (hltas)
add_subdirectory (hlstrafe)
add_subdirectory (taslogger)

include_directories ("." hltas/src hlstrafe/src taslogger)

set (LIBRARY_OUTPUT_DIRECTORY ".")
set (WINDOWS_FILES
	SPTLib/Windows/DetoursUtils.cpp
	SPTLib/Windows/Hooks_win.cpp
	SPTLib/Windows/MemUtils_win.cpp
	BunnymodXT/Windows/conutils.cpp
	BunnymodXT/Windows/dllmain.cpp
	BunnymodXT/Windows/interprocess.cpp)
set (LINUX_FILES
	SPTLib/Linux/Hooks_linux.cpp
	SPTLib/Linux/MemUtils_linux.cpp
	BunnymodXT/modules/PMSharedHooks_linux.cpp
	BunnymodXT/Linux/main_linux.cpp
	BunnymodXT/Linux/interprocess.cpp)
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
	BunnymodXT/hud_custom.cpp)

if (MSVC)
	include_external_msproject (MinHook "${CMAKE_SOURCE_DIR}/SPTLib/Windows/minhook/build/VC14/libMinHook.vcxproj")
	include_directories (
		"SPTLib/Windows/minhook/include"
		${RapidJSON_INCLUDE_DIR})
	set (SOURCE_FILES ${SOURCE_FILES} ${WINDOWS_FILES})
else ()
	set (SOURCE_FILES ${SOURCE_FILES} ${LINUX_FILES})
endif ()

add_library (BunnymodXT SHARED ${SOURCE_FILES})
set_target_properties(BunnymodXT PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "BunnymodXT/stdafx.hpp")
cotire(BunnymodXT)

target_link_libraries (BunnymodXT hltas hlstrafe taslogger ${Boost_LIBRARIES})
if (NOT MSVC)
	target_link_libraries (BunnymodXT pthread rt)
endif ()
if (MSVC)
	add_dependencies (BunnymodXT MinHook)
endif ()
