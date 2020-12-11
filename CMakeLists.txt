cmake_minimum_required(VERSION 3.17)
project(Ext2_FS C)

set(CMAKE_C_STANDARD 11)

add_executable(Ext2_FS main.c disk.h disk.c fs.h fs.c testModule.c testModule.h)