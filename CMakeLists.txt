CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(sarf)

set(build_dir ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(lib)
add_subdirectory(src)