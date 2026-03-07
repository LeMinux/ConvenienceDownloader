BINARY:=download_video_test.out
OBJECT_DIR:=./objects
TEST_CODE_DIR:=./source
SOURCE_CODE_DIR:=../../../../source
INCLUDES:=-I../../../../includes -I./includes

CC:=gcc
COMPILER_FLAG:=-DUSE_TESTING_DB
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings

DIR_ENV:=./dir_env
TEST_DIRS:=1 2 3 4

TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:= $(SOURCE_CODE_DIR)/databaseOps.c $(SOURCE_CODE_DIR)/userInput.c $(SOURCE_CODE_DIR)/fileOps.c $(SOURCE_CODE_DIR)/downloading.c $(SOURCE_CODE_DIR)/testSetups.c $(SOURCE_CODE_DIR)/writeArt.c $(SOURCE_CODE_DIR)/execOthers.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))

all: $(BINARY)

$(DIR_ENV):
	for dir in $(TEST_DIRS); do mkdir -p $(DIR_ENV)/$$dir; done

$(BINARY): $(O_FILES) | $(DIR_ENV)
	$(CC) -g -o $@ $^ -lcmocka -lsqlite3

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: $(TEST_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

run:
	for dir in $(TEST_DIRS); do rm -f $(DIR_ENV)/$$dir/*; done
	./$(BINARY)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(OBJECT_DIR)/*.d
	rm -f ./*.d
	rm -f $(BINARY)
	rm -rf $(DIR_ENV)
