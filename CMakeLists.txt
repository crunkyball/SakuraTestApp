project(SakuraTestApp)
cmake_minimum_required(VERSION 3.24)

if (WORKING_FROM_TOP_LEVEL)
    message(FATAL_ERROR "Already working from top level. If including from another project then use the Source directory.")
endif()

set(WORKING_FROM_TOP_LEVEL ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build)

#Ensure that Sakura is found and set up requirements.
find_path(SAKURA_SRC_DIR Sakura/SaCore.h PATHS ../Sakura/Source)

if (EXISTS ${SAKURA_SRC_DIR}/Sakura/SaCore.h)
    message("Sakura source directory found: " ${SAKURA_SRC_DIR})
else()
    message(FATAL_ERROR "Sakura source dir not found! Please ensure that SAKURA_SRC_DIR is set to the correct location.")
endif()

include(${SAKURA_SRC_DIR}/CMake_SakuraApp.txt)

add_subdirectory(${SAKURA_SRC_DIR} "${CMAKE_BINARY_DIR}/Sakura")

get_directory_property(SakuraIncludeDirs DIRECTORY ${SAKURA_SRC_DIR} INCLUDE_DIRECTORIES)
include_directories(${SakuraIncludeDirs})

add_subdirectory(Source)
