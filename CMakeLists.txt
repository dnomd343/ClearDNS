cmake_minimum_required(VERSION 2.8.12)

project(cleardns LANGUAGES C)

set(CMAKE_C_STANDARD 99)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

###############################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

###############################################################

macro(git_tag _tag)
    find_package(Git QUIET)
    if (GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags
            OUTPUT_VARIABLE ${_tag}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
    endif()
endmacro()

set(VERSION "")
git_tag(VERSION)
if(VERSION STREQUAL "")  # without git tag
    message(FATAL_ERROR "Unable to get version information")
endif()

###############################################################

add_subdirectory(src)

###############################################################
