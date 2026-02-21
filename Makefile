BINARY:=new_downloader
OBJECT_DIR:=./objects
CODE_DIR:=./source
INC_DIR:=./includes
TEST_DIR:=./test
CONFIG_DIR:=~/.config/con-downloader
DATABASE_FILE:=$(CONFIG_DIR)/con-downloader.db

CC:=gcc
DEP_FLAGS:=-MP -MD
#-Wconversion is too restrictive, but if you want to use it go ahead :P
#C_FLAGS:=-Wall -Werror -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wconversion -I$(INC_DIR) $(DEP_FLAGS)
C_FLAGS:=-Wall -Werror -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings -I$(INC_DIR) $(DEP_FLAGS)
C_END_FLAGS:=-lsqlite3
DEBUG_FLAGS:=-g -fsanitize=address -fsanitize-recover=address

C_FILES:=$(filter-out $(CODE_DIR)/test%, $(wildcard $(CODE_DIR)/*.c))
#C_FILES :=$(foreach dir, $(CODE_DIR),$(wildcard $(dir)/*.c))
O_FILES :=$(patsubst $(TEST_CODE_DIR)/%.c, $(OBJECT_DIR)/%.o, $(C_FILES))
DEP_FILES :=$(patsubst %.c, %.d, $(C_FILES))

#default
all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) $(C_FLAGS) -o $@ $^ -lsqlite3

#implicitly specify how to turn c files into object files
$(OBJECT_DIR)/%.o : $(CODE_DIR)/%.c
	$(CC) -c $(C_FLAGS) $< -o $@

#create database file
init:
	mkdir -p $(CONFIG_DIR)
	sqlite3 $(DATABASE_FILE) < $(CODE_DIR)/initDB.sql

#runs binary
run:
	./$(BINARY)

#compile with debuging information and address sanitization
leak: $(C_FILES)
	$(CC) $(C_FLAGS) $(DEBUG_FLAGS) -o $@ $^

#compile for gdb debugging
debug: $(C_FILES)
	$(CC) -g $(C_FLAGS) -o $@ $^

test-compile:
	$(MAKE) -C $(TEST_DIR) compile

test-run:
	$(MAKE) -C $(TEST_DIR) run

test-show-all:
	$(MAKE) -C $(TEST_DIR) run-no-supress

test-clean:
	$(MAKE) -C $(TEST_DIR) clean

#remove all object files, dependency files, and binary
clean:
	rm -f ./$(BINARY)
	rm -f ./$(OBJECT_DIR)/*.o
	rm -f ./$(OBJECT_DIR)/*.d
	rm -f ./*.d

removed-database:
	rm -f $(DATABASE_FILE)

#include the dependency files
-include $(DEP_FILES)

.PHONY: test

