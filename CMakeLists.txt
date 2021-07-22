CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(sarf)

add_compile_options(-Wall -Wextra -g -Os)
add_compile_definitions(LSARF_BUILT_OS="${CMAKE_SYSTEM_NAME}")

list(APPEND SOURCE_FILES
	lib/libtest.c
)

list(APPEND LIB_SOURCE_FILES
	lib/libsarf.c
	lib/archive.c
	lib/errors.c
	lib/utils.c
)

list(APPEND LIB_HEADER_FILES
	lib/libsarf.h
	lib/archive.h
	lib/errors.h
	lib/flags.h
	lib/utils.h
)

add_library(sarf ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})

add_executable(libtest ${SOURCE_FILES})
target_link_libraries(libtest PUBLIC sarf)
