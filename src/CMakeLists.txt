cmake_minimum_required(VERSION 2.8.12)

include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
add_executable(cleardns ${SRC})
#target_link_libraries(cleardns glib-2.0)
