cmake_minimum_required(VERSION 2.8.4)
project(BunnymodXT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m32")

include_directories(".")

set(LIBRARY_OUTPUT_DIRECTORY ".")

set(SOURCE_FILES
    BunnymodXT/modules/ClientDLL.cpp
    BunnymodXT/modules/ServerDLL.cpp
    BunnymodXT/cvars.cpp
    BunnymodXT/hud_custom.cpp
    BunnymodXT/modules.cpp
    SPTLib/IHookableModule.cpp
    SPTLib/Hooks.cpp
    SPTLib/MemUtils.cpp
    SPTLib/IHookableDirFilter.cpp
    SPTLib/IHookableNameFilter.cpp
    SPTLib/sptlib.cpp)

add_library(BunnymodXT SHARED ${SOURCE_FILES})