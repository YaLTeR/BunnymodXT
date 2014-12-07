cmake_minimum_required(VERSION 2.8.4)
project(BunnymodXT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m32 -g -Wno-write-strings")

include_directories(".")

set(LIBRARY_OUTPUT_DIRECTORY ".")

set(SOURCE_FILES
    SPTLib/IHookableModule.cpp
    SPTLib/IHookableDirFilter.cpp
    SPTLib/IHookableNameFilter.cpp
    SPTLib/Hooks.cpp
    SPTLib/Linux/Hooks_linux.cpp
    SPTLib/MemUtils.cpp
    SPTLib/Linux/MemUtils_linux.cpp
    SPTLib/sptlib.cpp
    BunnymodXT/modules/ClientDLL.cpp
    BunnymodXT/modules/ServerDLL.cpp
    BunnymodXT/modules/PMSharedHooks_linux.cpp
    BunnymodXT/cvars.cpp
    BunnymodXT/hud_custom.cpp
    BunnymodXT/modules.cpp
    BunnymodXT/Linux/main_linux.cpp)

add_library(BunnymodXT SHARED ${SOURCE_FILES})