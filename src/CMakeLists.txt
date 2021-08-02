CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(sarf)

add_compile_options(-Wall -Wextra -g -Os)
add_compile_definitions(LSARF_BUILT_OS="${CMAKE_SYSTEM_NAME}")

list(APPEND SOURCE_FILES
	sarf.c
	utils.c
)

find_library(LIBSARF REQUIRED
    NAMES sarf
    HINTS "${build_dir}/lib"
)

add_executable(libtest ${SOURCE_FILES})
target_include_directories(libtest PUBLIC "../lib")
target_link_libraries(libtest ${LIBSARF})
