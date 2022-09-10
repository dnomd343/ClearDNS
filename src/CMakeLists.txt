cmake_minimum_required(VERSION 2.8.12)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/utils)

add_subdirectory(utils)

add_executable(cleardns cleardns.c)
target_link_libraries(cleardns utils)
