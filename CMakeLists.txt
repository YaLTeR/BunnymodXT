cmake_minimum_required (VERSION 3.12)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

if (NOT CMAKE_GENERATOR MATCHES "Visual Studio .+")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif ()

# Call project() after setting -m32 so it is taken into account.
# Specifically, this allows CMAKE_SIZEOF_VOID_P to get the correct 32-bit value.
project (BunnymodXT)

if (MSVC)
	if (MSVC_TOOLSET_VERSION MATCHES "141")
		set (MINHOOK_PROJECT_DIR "VC15")
	elseif (MSVC_TOOLSET_VERSION MATCHES "142")
		set (MINHOOK_PROJECT_DIR "VC16")
	else ()
		message (FATAL_ERROR "Visual Studio 2017 or 2019 is required")
	endif()

	if (NOT CMAKE_GENERATOR_PLATFORM MATCHES "Win32")
		message (FATAL_ERROR "Only the Win32 platform is supported. Pass -A Win32 to CMake")
	endif ()

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
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=pentium-m -mfpmath=387 -mno-sse -g -Wall -Wextra -Wno-unused-parameter")

	if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		# Not supported on Clang.
		# Required for reproducibility of hlstrafe between different optimization levels.
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffloat-store")
	endif ()

	if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
		option (GLIBCXX_DEBUG "Enable libstdc++ debug checks." ON)
	else ()
		option (GLIBCXX_DEBUG "Enable libstdc++ debug checks." OFF)
	endif ()

	set(CMAKE_SHARED_LINKER_FLAGS
		"${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif ()

include(cotire)

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_TAG --tags)
git_local_changes(GIT_LOCAL_CHANGES)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/BunnymodXT/git_revision.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/BunnymodXT/git_revision.cpp" @ONLY)

find_package (Boost 1.57.0 REQUIRED)
include_directories (SYSTEM ${Boost_INCLUDE_DIR})

set (RapidJSON_ROOT "${CMAKE_SOURCE_DIR}/rapidjson")

# hltas is already included as part of hlstrafe.
add_subdirectory (hlstrafe)
add_subdirectory (taslogger)

include_directories ("." taslogger)
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
	BunnymodXT/modules/SDL.hpp
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
	BunnymodXT/opengl_utils.hpp
	BunnymodXT/TEA.hpp
	BunnymodXT/runtime_data.hpp
	BunnymodXT/input_editor.hpp
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
	BunnymodXT/modules/SDL.cpp
	BunnymodXT/custom_triggers.cpp
	BunnymodXT/cvars.cpp
	BunnymodXT/hud_custom.cpp
	BunnymodXT/triangle_drawing.cpp
	BunnymodXT/triangle_utils.cpp
	BunnymodXT/opengl_utils.cpp
	BunnymodXT/TEA.cpp
	BunnymodXT/runtime_data.cpp
	BunnymodXT/input_editor.cpp
	${CMAKE_CURRENT_BINARY_DIR}/BunnymodXT/git_revision.cpp)

if (MSVC)
	include_external_msproject (MinHook "${CMAKE_SOURCE_DIR}/SPTLib/Windows/minhook/build/${MINHOOK_PROJECT_DIR}/libMinHook.vcxproj")
	include_directories ("SPTLib/Windows/minhook/include")
	set (SOURCE_FILES ${SOURCE_FILES} ${WINDOWS_FILES} ${HEADER_FILES})
else ()
	set (SOURCE_FILES ${SOURCE_FILES} ${LINUX_FILES} ${HEADER_FILES})
endif ()

add_library (BunnymodXT SHARED ${SOURCE_FILES})
set_target_properties(BunnymodXT PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

# Interaction between Cotire and C++17 enforcement breaks Clang,
# and due to something severely outdated in Cotire GCC refuses to use its precompiled headers too.
if (MSVC)
	set_target_properties(BunnymodXT PROPERTIES
		COTIRE_CXX_PREFIX_HEADER_INIT "BunnymodXT/stdafx.hpp")
	cotire(BunnymodXT)
endif ()

if (NOT MSVC)
	if (GLIBCXX_DEBUG)
		# Don't enable on HLStrafe because the VCT becomes unusably slow with this.
		target_compile_definitions (BunnymodXT PRIVATE _GLIBCXX_DEBUG)
		# If I don't enable it on hltas-cpp, BXT crashes when trying to call HLTAS::Input::Clear().
		target_compile_definitions (hltas-cpp PRIVATE _GLIBCXX_DEBUG)
		message (STATUS "Enabled the libstdc++ debug mode")
	endif ()
endif ()

target_link_libraries (BunnymodXT hltas-cpp hlstrafe taslogger)
if (NOT MSVC)
	target_link_libraries (BunnymodXT pthread rt)
else()
	add_dependencies (BunnymodXT MinHook)
	target_link_libraries (BunnymodXT opengl32)
endif ()

install (TARGETS BunnymodXT)
