SUBDIRS:=./*/

all: $(SUBDIRS)
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done

clean:
	for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir clean;\
	done

run:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run | grep --extended-regexp --ignore-case "FAILED|ERROR";\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done

run-no-supress:
	@for dir in $(SUBDIRS); do\
		printf "\n%s\n" "Running $$dir";\
		printf "%s\n" "-------------------------";\
		$(MAKE) -C $$dir run-no-supress;\
		printf "%s\n" "~~~~~~~~~~~~~~~~~~~~~~~~~";\
	done
