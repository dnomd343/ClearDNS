cmake_minimum_required(VERSION 2.8.12)

add_library(common json.c sundry.c system.c structure.c)
target_link_libraries(common to_json)
