SUBDIRS:=./*/

input-all: $(SUBDIRS)
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done

input-clean:
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir clean;\
	done

run: input-run

input-run:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run | grep --extended-regexp --ignore-case "FAILED|ERROR";\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done

input-run-no-supress:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run;\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done
