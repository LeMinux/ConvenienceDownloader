#note the dot after the period. This is to specify the current directory
BINARY:=downloader
CODE_DIR:=.
INC_DIR:=.

CC :=gcc
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Werror -Wpedantic $(foreach dir, $(INC_DIR),-I$(dir)) $(DEP_FLAGS)
DEBUG_FLAGS :=-g -fsanitize=address -fsanitize-recover=address

C_FILES :=$(foreach dir, $(CODE_DIR),$(wildcard $(dir)/*.c))
O_FILES :=$(patsubst %.c, %.o, $(C_FILES))
DEP_FILES :=$(patsubst %.c, %.d, $(C_FILES))

DIR :=./Destinations/

#default
all: $(BINARY)

#create an executable with the name downloader with passed in dependencies
$(BINARY): $(O_FILES)
	$(CC) $(C_FLAGS) -o $@ $^

#implicitly specify how to turn c files into object files
%.o : %.c
	$(CC) -c $(C_FLAGS) $< -o $@
	@ if [ ! -d "$(DIR)" ]; then \
		mkdir -p "$(DIR)"; \
	fi
	@chmod 700 $(DIR);

#runs binary
run:
	./$(BINARY)

#compile with debuging information
debug: $(O_FILES)
	$(CC) $(C_FLAGS) $(DEBUG_FLAGS) -o $@ $^

#remove all object files, dependency files, and binary
clean:
	rm -rf $(BINARY) $(O_FILES) $(DEP_FILES)

#include the dependency files
-include $(DEP_FILES)
