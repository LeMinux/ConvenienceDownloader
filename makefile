# makefile for music downloader

CFILES:= cMusicDownload.c linkedList.c helpers.c

test-run: test-compile
	./test.out
true-run: true-compile
	./download.out
debug-run: debug-compile
	./debug.out
debug-gdb-run: debug-gdb
	gdb gdb.out

test-compile:
	gcc -Wall -o test.out $(CFILES)
debug-compile:
	gcc -Wall -fsanitize=address -fsanitize-recover=address -o debug.out $(CFILES)
debug-gdb:
	gcc -g -Wall -o gdb.out $(CFILES)
true-compile:
	gcc -Wall -o download.out $(CFILES)

test-clean:
	rm test.out
true-clean:
	rm download.out
debug-clean:
	rm debug.out
clean-all:
	rm *.out
