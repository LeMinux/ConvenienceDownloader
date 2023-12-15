# makefile for music downloader

CFILES:=musicDriver.c cMusicDownload.c linkedList.c helpers.c writeArt.c
DIR:=Destinations

all:
	gcc $(CFILES) -o testing.out
	if [ ! -d "$(DIR)" ]; then \
		mkdir -p "$(DIR)"; \
	fi

test:
	gcc -Wall -o test.out $(CFILES)
	./test.out

leaks:
	gcc -g -Wall -fsanitize=address -fsanitize-recover=address -o debug.out $(CFILES)
	ASAN_OPTIONS=symbolize=1 ./debug.out

debug:
	gcc -g -Wall -fsanitize=address -fsanitize-recover=address -o debug.out $(CFILES)
	gdb gdb.out

clean:
	rm *.out
