cmake_minimum_required(VERSION 2.8.12)

include_directories(${PROJECT_SOURCE_DIR}/include/bcrypt/blowfish)

add_subdirectory(blowfish)

add_library(bcrypt bcrypt.c hash.c)
target_link_libraries(bcrypt blowfish)
