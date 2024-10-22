CC=gcc
CFLAGS=-ansi -g

cs240StreamingService: main.c streaming_service.h movie.c
	$(CC) $(CFLAGS) main.c movie.c -o $@

.PHONY: clean

clean:
	rm -f cs240StreamingService
