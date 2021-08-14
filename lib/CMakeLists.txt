CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(libsarf)

add_compile_options(-Wall -Wextra -g -Os)
add_compile_definitions(LSARF_BUILT_OS="${CMAKE_SYSTEM_NAME}")

list(APPEND LIB_SOURCE_FILES
	sarf_close.c
	sarf_entry.c	
	sarf_error.c
	sarf_init.c
	sarf_open.c
	sarf_read_entry.c
	sarf_read.c
	sarf_skip_file_data.c	
	sarf_write_entry.c
	sarf_write.c
)

list(APPEND LIB_HEADER_FILES
	sarf.h
	sarf_int.h
)

add_library(sarf ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})
