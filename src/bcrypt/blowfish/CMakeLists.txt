cmake_minimum_required(VERSION 2.8.12)

add_library(blowfish crypt_blowfish.c crypt_gensalt.c wrapper.c x86.S)
