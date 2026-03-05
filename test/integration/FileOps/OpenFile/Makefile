BINARY:=open_file_integration.out
OBJECT_DIR:=./objects
TEST_CODE_DIR:=./source
SOURCE_CODE_DIR:=../../../../source
INCLUDES:=-I../../../../includes -I./includes

TEST_ENV:=test_env
TEST_DIR:=test_dir
TEST_FILE:=test_file
TEST_DIR_LINK:=$(TEST_ENV)/link_test_dir
TEST_FILE_LINK:=$(TEST_ENV)/link_test_file
TEST_BROKEN_LINK:=$(TEST_ENV)/link_broken
TEST_NO_PERM_LINK:=$(TEST_ENV)/link_no_perm
TEST_NON_EXIST:=$(TEST_ENV)/nope


CC:=gcc
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings
MOCK_FLAGS:=-Wl,--wrap=boundedInput

TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:=$(SOURCE_CODE_DIR)/fileOps.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))

all: $(BINARY) | $(TEST_ENV)

$(BINARY): $(O_FILES)
	$(CC) -o $@ $^ -lcmocka

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(TEST_ENV):
	mkdir -p $(TEST_ENV)

	#absolute and relative testing
	mkdir -p $(TEST_ENV)/$(TEST_DIR)
	touch $(TEST_ENV)/$(TEST_FILE)

	#link testing
	ln -fs ./$(TEST_DIR) $(TEST_DIR_LINK)
	ln -fs ./$(TEST_FILE) $(TEST_FILE_LINK)
	ln -fs /root $(TEST_NO_PERM_LINK)
	ln -fs $(TEST_NON_EXIST) $(TEST_BROKEN_LINK)

$(OBJECT_DIR)/%.o: $(TEST_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

run:
	./$(BINARY)

clean:
	rm $(OBJECT_DIR)/*.o
	rm $(OBJECT_DIR)/*.d
	rm -rf $(TEST_ENV)
	rm *.out
