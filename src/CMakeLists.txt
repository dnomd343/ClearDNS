cmake_minimum_required(VERSION 2.8.12)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/include/applet)
include_directories(${PROJECT_SOURCE_DIR}/include/bcrypt)
include_directories(${PROJECT_SOURCE_DIR}/include/common)
include_directories(${PROJECT_SOURCE_DIR}/include/loader)

link_directories(${PROJECT_SOURCE_DIR}/src/target/release)

configure_file(
    ${PROJECT_SOURCE_DIR}/include/constant.h.in
    ${PROJECT_SOURCE_DIR}/include/constant.h
)

add_subdirectory(utils)
add_subdirectory(applet)
add_subdirectory(bcrypt)
add_subdirectory(common)
add_subdirectory(loader)

add_executable(cleardns cleardns.c)
target_link_libraries(cleardns utils applet bcrypt common loader)
