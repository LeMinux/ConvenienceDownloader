SUBDIRS:=./*/

unit-all: $(SUBDIRS)
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done

unit-clean:
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir clean;\
	done

unit-run:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run | grep --extended-regexp --ignore-case "FAILED|ERROR";\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done

unit-run-no-supress:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run;\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done
