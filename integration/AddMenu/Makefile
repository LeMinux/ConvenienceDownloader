BINARY:=update_menu_integration_test.out
OBJECT_DIR:=./test_objects
TEST_CODE_DIR:=./test_source
SOURCE_CODE_DIR:=../../../source
INCLUDES:=-I../../../includes -I./test_includes -I../menu_includes/

CC:=gcc
COMPILER_FLAG:=-DTESTING
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings
MOCK_FLAGS:=-Wl,--wrap=boundedInput,--wrap=takeDirectoryInput,--wrap=takeDepthInput,--wrap=takeIndexInput


TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:=$(SOURCE_CODE_DIR)/databaseUserInput.c $(SOURCE_CODE_DIR)/databaseOps.c $(SOURCE_CODE_DIR)/userInput.c $(SOURCE_CODE_DIR)/fileOps.c $(SOURCE_CODE_DIR)/testWrapInput.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))


TEST_ENV:=dir_env

ROOT1:=$(TEST_ENV)/root1
ROOT2:=$(TEST_ENV)/root2

1LEVEL_LINK:=link1
1LEVEL_FILE:=file1
1LEVEL_LEFT_DIR:=left_dir
1LEVEL_RIGHT_DIR:=right_dir

2LEVEL_LEFT_DIR:=inner_left
2LEVEL_RIGHT_DIR:=inner_right

3LEVEL_DIR:=inner_inner_dir

$(TEST_ENV):
	mkdir -p ./$(TEST_ENV)

	#create first root env
	mkdir -p ./$(ROOT1)/$(1LEVEL_LEFT_DIR)
	mkdir -p ./$(ROOT1)/$(1LEVEL_RIGHT_DIR)

	touch ./$(ROOT1)/$(1LEVEL_FILE)
	ln -sf ./$(1LEVEL_RIGHT_DIR) $(ROOT1)/$(1LEVEL_LINK)


	mkdir -p ./$(ROOT1)/$(1LEVEL_LEFT_DIR)/$(2LEVEL_LEFT_DIR)
	mkdir -p ./$(ROOT1)/$(1LEVEL_LEFT_DIR)/$(2LEVEL_RIGHT_DIR)

	mkdir -p ./$(ROOT1)/$(1LEVEL_RIGHT_DIR)/$(2LEVEL_LEFT_DIR)
	mkdir -p ./$(ROOT1)/$(1LEVEL_RIGHT_DIR)/$(2LEVEL_RIGHT_DIR)

	mkdir -p ./$(ROOT1)/$(1LEVEL_LEFT_DIR)/$(2LEVEL_RIGHT_DIR)/$(3LEVEL_DIR)

	mkdir -p ./$(ROOT1)/$(1LEVEL_RIGHT_DIR)/$(2LEVEL_LEFT_DIR)/$(3LEVEL_DIR)

	#create second root env
	mkdir -p $(ROOT2)
	for number in 1 2 3 4 5 6 7; do \
		touch "$(ROOT2)/file$$number"; \
	done

all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) -g $(COMPILER_FLAG) $(MOCK_FLAGS) $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) -o $@ $^ -lcmocka -lsqlite3

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: $(TEST_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

run: $(TEST_ENV)
	./$(BINARY)

clean:
	rm -rf ./$(TEST_ENV)
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(OBJECT_DIR)/*.d
	rm -f ./*.d
	rm -f $(BINARY)

