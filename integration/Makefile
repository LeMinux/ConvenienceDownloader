SUBDIRS:=./*/

int-all: $(SUBDIRS)
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done

int-clean:
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir clean;\
	done

int-run:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run | grep --extended-regexp --ignore-case "FAILED|ERROR";\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done

int-run-no-supress:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run;\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done
