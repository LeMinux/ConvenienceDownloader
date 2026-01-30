SUBDIRS:=./TakeDepthInput/ ./TakeIndexInput//

take-all: $(SUBDIRS)
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done

take-clean:
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir clean;\
	done

run: take-run

take-run:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir\n";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run | grep --extended-regexp --ignore-case "FAILED|ERROR";\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done

take-run-no-supress:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir\n";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run;\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done
