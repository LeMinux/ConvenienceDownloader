BINARY:=database_ops_unit_test.out
OBJECT_DIR:=./test_objects
TEST_CODE_DIR:=./test_source

CC:=gcc
COMPILER_FLAG:=-DTESTING
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings

C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c) ../../source/databaseOps.c ../../source/userInput.c
O_FILES := $(patsubst %.c, $(OBJECT_DIR)/%.o, $(notdir $(C_FILES)))
DEP_FILES :=$(patsubst %.c, %.d, $(C_FILES))

all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) $(COMPILER_FLAG) $(C_FLAGS) $(DEP_FLAGS) -I../../includes/ -o $@ $^ -lcmocka -lsqlite3

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

%.o : %.c | $(OBJECT_DIR)
	$(CC) -c $(COMPILER_FLAG) $(C_FLAGS) $< -o $(OBJECT_DIR)/$(notdir $@)

#$(OBJECT_DIR)/%.o : ../../source/%.c# | $(OBJECT_DIR)
#	$(CC) -c $(C_FLAGS) $< -o $@

run:
	./$(BINARY)
