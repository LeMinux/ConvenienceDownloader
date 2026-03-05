BINARY:=refresh_database_integration_test.out
OBJECT_DIR:=./objects
TEST_CODE_DIR:=./source
SOURCE_CODE_DIR:=../../../../source
INCLUDES:=-I../../../../includes -I./includes

CC:=gcc
COMPILER_FLAG:=-DTESTING
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings

DIR_ENV:=./dir_env

CONSTANT_ROOT:=$(DIR_ENV)/constant
ADDED_CONTENT_ROOT:=$(DIR_ENV)/added_stuff
REMOVED_CONTENT_ROOT:=$(DIR_ENV)/removed_stuff
BLACK_ROOT:=$(DIR_ENV)/blacklist

TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:=$(SOURCE_CODE_DIR)/databaseOps.c $(SOURCE_CODE_DIR)/userInput.c $(SOURCE_CODE_DIR)/fileOps.c $(SOURCE_CODE_DIR)/testSetups.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))

all: $(BINARY)

$(DIR_ENV):
	mkdir -p $(CONSTANT_ROOT)
	mkdir -p $(CONSTANT_ROOT)/1
	mkdir -p $(CONSTANT_ROOT)/2
	mkdir -p $(CONSTANT_ROOT)/3

	mkdir -p $(ADDED_CONTENT_ROOT)
	mkdir -p $(ADDED_CONTENT_ROOT)/1
	mkdir -p $(ADDED_CONTENT_ROOT)/2
	mkdir -p $(ADDED_CONTENT_ROOT)/3
	mkdir -p $(ADDED_CONTENT_ROOT)/4

	mkdir -p $(REMOVED_CONTENT_ROOT)
	mkdir -p $(REMOVED_CONTENT_ROOT)/1
	mkdir -p $(REMOVED_CONTENT_ROOT)/2

	mkdir -p $(BLACK_ROOT)
	mkdir -p $(BLACK_ROOT)/1

$(BINARY): $(O_FILES) | $(DIR_ENV)
	$(CC) -g -o $@ $^ -lcmocka -lsqlite3

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: $(TEST_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

run:
	./$(BINARY)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(OBJECT_DIR)/*.d
	rm -f ./*.d
	rm -f $(BINARY)
	rm -rf $(DIR_ENV)


