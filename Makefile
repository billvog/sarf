CC = clang
CFLAGS = -Wall -Wextra -g

LIB_SRC_FILES = lib/sarf.c
LIB_INCLUDE_FILE = lib/sarf.h

all: libtest

libsarf.o: $(LIB_SRC_FILES) $(LIB_INCLUDE_FILE)
	$(CC) $(CFLAGS) -c lib/sarf.c -o $@

libtest: lib/libtest.c libsarf.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o libtest