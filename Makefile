detected_OS := $(shell uname 2>/dev/null || echo Unknown)
detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))

ifeq ($(detected_OS),Windows)
    BUILT_FOR_OS += WIN32
endif
ifeq ($(detected_OS),Darwin)
    BUILT_FOR_OS += Apple
endif
ifeq ($(detected_OS),Linux)
    BUILT_FOR_OS += Linux
endif
ifeq ($(detected_OS),FreeBSD)
    BUILT_FOR_OS += FreeBSD
endif

CC = clang
CFLAGS = -Wall -Wextra -g
CFLAGS += -DLSARF_BUILT_OS=\"$(BUILT_FOR_OS)\"


LIB_SRC_FILES = lib/sarf.c
LIB_INCLUDE_FILE = lib/sarf.h

all: libtest

libsarf.o: $(LIB_SRC_FILES) $(LIB_INCLUDE_FILE)
	$(CC) $(CFLAGS) -c lib/sarf.c -o $@

libtest: lib/libtest.c libsarf.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o libtest