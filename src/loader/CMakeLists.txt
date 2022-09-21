cmake_minimum_required(VERSION 2.8.12)

add_library(loader config.c default.c loader.c parser.c)
target_link_libraries(loader applet common)
