cmake_minimum_required(VERSION 3.16)
project(hw3)

set(CMAKE_CXX_STANDARD 11)

add_executable(hw3 main.cpp fat32.h helper_printer.cpp helper_reader.cpp helper_writer.cpp example.img)
