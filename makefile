# makefile for music downloader

CFILES:=musicDriver.c userInput.c linkedList.c helpers.c writeArt.c fileOps.c
DIR:=Destinations

all:
	@gcc $(CFILES) -o download.out
	@echo "Compiled okay"
	@if [ ! -d "$(DIR)" ]; then \
		mkdir -p "$(DIR)"; \
		echo "Destinations directory has been made"; \
	else \
		echo "Destinations directory already exists"; \
	fi

run:
	./download.out

leaks:
	gcc -g -Wall -fsanitize=address -fsanitize-recover=address -o debug.out $(CFILES)
	ASAN_OPTIONS=symbolize=1 ./debug.out

debug:
	gcc -g -Wall -fsanitize=address -fsanitize-recover=address -o debug.out $(CFILES)
	gdb gdb.out

clean:
	rm *.out
