CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(libsarf)

add_compile_options(-Wall -Wextra -g -Os)
add_compile_definitions(LSARF_BUILT_OS="${CMAKE_SYSTEM_NAME}")

list(APPEND LIB_SOURCE_FILES
	libsarf_add_dir.c
	libsarf_add_file.c
	libsarf_close.c
	libsarf_count_entries.c
	libsarf_err2str.c
	libsarf_extract.c
	libsarf_init.c
	libsarf_open.c
	libsarf_read_entry.c
	libsarf_remove.c
	libsarf_stat.c
)

list(APPEND LIB_HEADER_FILES
	sarf.h
	sarf_int.h
)

add_library(sarf ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})
