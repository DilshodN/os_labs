cmake_minimum_required(VERSION 3.20)
project(task30 C)

set(CMAKE_C_STANDARD 11)

add_executable(task30 ../task31/client.c server.c)
