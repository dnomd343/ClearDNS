cmake_minimum_required(VERSION 2.8.12)

add_library(applet adguard.c dnsproxy.c overture.c)
target_link_libraries(applet utils)
