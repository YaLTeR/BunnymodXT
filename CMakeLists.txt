cmake_minimum_required (VERSION 3.1)
project (BunnymodXT)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

if (MSVC)
	# Enable link-time code generation.
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} /GL")
	set (CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS} ${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG")
	set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG:incremental")

	# Switch to the static runtime.
	set (flag_variables
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
	)

	foreach (variable ${flag_variables})
		if (${variable} MATCHES "/MD")
			string (REPLACE "/MD" "/MT" ${variable} "${${variable}}")
		endif ()
	endforeach ()

	# If we're building for Windows XP, add a special compiler flag to fix a crash on Windows XP.
	# http://stackoverflow.com/a/32953859/4214632
	if (CMAKE_GENERATOR_TOOLSET MATCHES "_xp")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:threadSafeInit-")
	endif ()
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=native -mtune=native -m32 -g -Ofast -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-variable -Wno-missing-braces")
	#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=pentium-m -mfpmath=387 -m32 -g -Ofast -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-variable -Wno-missing-braces")

	# On Clang this causes warnings when compiling precompiled headers (done by Cotire).
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
endif ()

include(cotire)

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_TAG --tags)
git_local_changes(GIT_LOCAL_CHANGES)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/BunnymodXT/git_revision.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/BunnymodXT/git_revision.cpp" @ONLY)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME ON)
find_package (Boost 1.57.0 REQUIRED)
include_directories (SYSTEM ${Boost_INCLUDE_DIR})

add_subdirectory (hltas)
add_subdirectory (hlstrafe)
add_subdirectory (taslogger)

include_directories ("." hltas/src hlstrafe/src taslogger)
include_directories (SYSTEM ${RapidJSON_INCLUDE_DIR})
include_directories (SYSTEM cereal/include)

set (LIBRARY_OUTPUT_DIRECTORY ".")
set (WINDOWS_FILES
	SPTLib/Windows/DetoursUtils.cpp
	SPTLib/Windows/Hooks_win.cpp
	SPTLib/Windows/MemUtils_win.cpp
	SPTLib/Windows/DetoursUtils.hpp
	BunnymodXT/Windows/conutils.cpp
	BunnymodXT/Windows/dllmain.cpp
	BunnymodXT/Windows/interprocess.cpp
	BunnymodXT/Windows/conutils.hpp)

set (LINUX_FILES
	SPTLib/Linux/Hooks_linux.cpp
	SPTLib/Linux/MemUtils_linux.cpp
	BunnymodXT/modules/PMSharedHooks_linux.cpp
	BunnymodXT/Linux/main_linux.cpp
	BunnymodXT/Linux/interprocess.cpp)

set (HEADER_FILES
	SPTLib/IHookableModule.hpp
	SPTLib/IHookableDirFilter.hpp
	SPTLib/IHookableNameFilter.hpp
	SPTLib/IHookableNameFilterOrdered.hpp
	SPTLib/Hooks.hpp
	SPTLib/MemUtils.hpp
	SPTLib/patterns.hpp
	SPTLib/sptlib.hpp
	BunnymodXT/modules/HwDLL.hpp
	BunnymodXT/modules/ClientDLL.hpp
	BunnymodXT/modules/ServerDLL.hpp
	BunnymodXT/bunnymodxt.hpp
	BunnymodXT/cmd_wrapper.hpp
	BunnymodXT/custom_triggers.hpp
	BunnymodXT/cvars.hpp
	BunnymodXT/hud_custom.hpp
	BunnymodXT/interprocess.hpp
	BunnymodXT/modules.hpp
	BunnymodXT/patterns.hpp
	BunnymodXT/shared.hpp
	BunnymodXT/sptlib-wrapper.hpp
	BunnymodXT/stdafx.hpp
	BunnymodXT/triangle_drawing.hpp
	BunnymodXT/triangle_utils.hpp
	BunnymodXT/TEA.hpp
	BunnymodXT/runtime_data.hpp
	BunnymodXT/git_revision.hpp)

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
	BunnymodXT/custom_triggers.cpp
	BunnymodXT/cvars.cpp
	BunnymodXT/hud_custom.cpp
	BunnymodXT/triangle_drawing.cpp
	BunnymodXT/triangle_utils.cpp
	BunnymodXT/TEA.cpp
	BunnymodXT/runtime_data.cpp
	${CMAKE_CURRENT_BINARY_DIR}/BunnymodXT/git_revision.cpp)

if (MSVC)
	include_external_msproject (MinHook "${CMAKE_SOURCE_DIR}/SPTLib/Windows/minhook/build/VC14/libMinHook.vcxproj")
	include_directories ("SPTLib/Windows/minhook/include")
	set (SOURCE_FILES ${SOURCE_FILES} ${WINDOWS_FILES} ${HEADER_FILES})
else ()
	set (SOURCE_FILES ${SOURCE_FILES} ${LINUX_FILES} ${HEADER_FILES})
endif ()

add_library (BunnymodXT SHARED ${SOURCE_FILES})
set_target_properties(BunnymodXT PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "BunnymodXT/stdafx.hpp")
cotire(BunnymodXT)

target_link_libraries (BunnymodXT hltas hlstrafe taslogger ${Boost_LIBRARIES})
if (NOT MSVC)
	target_link_libraries (BunnymodXT pthread rt)
else()
	add_dependencies (BunnymodXT MinHook)
	target_link_libraries (BunnymodXT opengl32)
endif ()
