cmake_minimum_required(VERSION 2.8.12)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/include/applet)
include_directories(${PROJECT_SOURCE_DIR}/include/loader)

add_subdirectory(utils)
add_subdirectory(applet)
add_subdirectory(loader)

add_executable(cleardns cleardns.c common.c)
target_link_libraries(cleardns utils applet loader)
