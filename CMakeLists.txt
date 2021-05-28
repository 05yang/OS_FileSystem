cmake_minimum_required(VERSION 3.17)
project(OS_FileSystem)

set(CMAKE_CXX_STANDARD 14)

add_executable(OS_FileSystem main.cpp FileSystem.cpp FileSystem.h Dir.cpp Dir.h Path.cpp Path.h Block.cpp Block.h Inode.cpp Inode.h)