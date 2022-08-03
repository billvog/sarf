list(APPEND LIBSARF_SOURCE_FILES
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

list(APPEND LIBSARF_HEADER_FILES
	sarf.h
	sarf_int.h
)

add_library(sarf STATIC ${LIBSARF_SOURCE_FILES})
target_include_directories(sarf PUBLIC ${LIBSARF_HEADER_FILES})