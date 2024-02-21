cmake_minimum_required(VERSION 3.27)
project(ipv6 C)

set(CMAKE_C_STANDARD 11)

add_library(ipv6 STATIC library.c
        echoServer.c)
