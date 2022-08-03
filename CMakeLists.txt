CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(libsarf)

add_compile_options(-Wall -Wextra -g -Os)
add_compile_definitions(LSARF_BUILT_OS="${CMAKE_SYSTEM_NAME}")

set(build_dir ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(lib)
add_subdirectory(libtest)