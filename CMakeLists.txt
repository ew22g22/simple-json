cmake_minimum_required(VERSION 3.0)
project(simple-json C)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE example_files "${PROJECT_SOURCE_DIR}/example/*.c")

add_executable(simple-json ${example_files} "${PROJECT_SOURCE_DIR}/src/sjson.c")
target_link_libraries(simple-json m)
