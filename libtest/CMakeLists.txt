list(APPEND LIBTEST_SOURCE_FILES
	sarf.c
	utils.c
)

list(APPEND LIBTEST_HEADER_FILES
	utils.h
)

add_executable(libtest ${LIBTEST_SOURCE_FILES})
target_include_directories(libtest PUBLIC ${LIBTEST_HEADER_FILES})

target_link_libraries(libtest PUBLIC sarf)
target_include_directories(libtest PUBLIC "../lib")
