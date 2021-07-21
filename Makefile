BUILT_FOR_OS = $(shell uname 2>/dev/null || echo Unknown)

CC = clang
CFLAGS = -Wall -Wextra -g
CFLAGS += -DLSARF_BUILT_OS=\"$(BUILT_FOR_OS)\"

LIB_SRC_FILES = lib/libsarf.c
LIB_INCLUDE_FILE = lib/libsarf.h

all: libtest

libsarf.o: $(LIB_SRC_FILES) $(LIB_INCLUDE_FILE)
	$(CC) $(CFLAGS) -c $(LIB_SRC_FILES) -o $@

libtest: lib/libtest.c libsarf.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o libtest libtest.dSYM