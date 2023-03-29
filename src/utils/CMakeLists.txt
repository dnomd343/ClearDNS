cmake_minimum_required(VERSION 2.8.12)

add_library(utils cJSON.c assets.c logger.c process.c)
target_link_libraries(utils assets)
