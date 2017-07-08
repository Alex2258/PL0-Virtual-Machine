cmake_minimum_required(VERSION 3.7)
project(parser)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES complier_driver.h lexical_analyser.h parser_code_generator.h main.c p_machine.h)
add_executable(parser ${SOURCE_FILES})