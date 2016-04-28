HEADERS = functions.h
CC = clang
CFLAGS = -lpcap -lpthread -g -O0

default: s_router

s_router: main.c
	$(CC) $(CFLAGS) main.c -o bin/s_router

debug:
	gdb -q --command=debug_commands.gdb bin/s_router

clean:
	-rm -rf bin/*

test:
	-rm bin/test
	$(CC) $(CFLAGS) test.c -o bin/test
	./bin/test
