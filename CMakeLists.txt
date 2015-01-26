cmake_minimum_required(VERSION 2.8.4)
project(BunnymodXT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -mtune=native -m32 -g -Wall -Wextra -Wno-unused-parameter")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=pentium-m -mfpmath=387 -m32 -g -Wall -Wextra -Wno-unused-parameter")

set (Boost_USE_STATIC_LIBS ON)
find_package (Boost 1.57.0 REQUIRED COMPONENTS chrono date_time system thread)
include_directories (${Boost_INCLUDE_DIR})

add_subdirectory (hltas)
add_subdirectory (hlstrafe)

include_directories("." hltas/src hlstrafe/src)

set(LIBRARY_OUTPUT_DIRECTORY ".")
set(SOURCE_FILES
    SPTLib/IHookableModule.cpp
    SPTLib/IHookableDirFilter.cpp
    SPTLib/IHookableNameFilter.cpp
    SPTLib/IHookableNameFilterOrdered.cpp
    SPTLib/Hooks.cpp
    SPTLib/Linux/Hooks_linux.cpp
    SPTLib/MemUtils.cpp
    SPTLib/Linux/MemUtils_linux.cpp
    SPTLib/sptlib.cpp
    BunnymodXT/modules/HwDLL.cpp
    BunnymodXT/modules/ClientDLL.cpp
    BunnymodXT/modules/ServerDLL.cpp
    BunnymodXT/modules/PMSharedHooks_linux.cpp
    BunnymodXT/cvars.cpp
    BunnymodXT/hud_custom.cpp
    BunnymodXT/Linux/main_linux.cpp)

add_library(BunnymodXT SHARED ${SOURCE_FILES})
target_link_libraries (BunnymodXT hltas hlstrafe ${Boost_LIBRARIES})
if (NOT MSVC)
	target_link_libraries (BunnymodXT pthread)
endif ()
